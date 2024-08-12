Shutting down gracefully is important for any long lasting process, especially for one that handles some kind of state. For example, what if you wanted to shutdown the database that supports your application and the db process didn't flush the current state to the disk, or what if you wanted to shut down a web server with thousands of connections but didn't wait for the requests to finish Not only does shutting down gracefully positively affect the user experience, it also eases internal operations, leading to happier engineers and less stressed SREs.

To shutdown gracefully is for the program to terminate after:

- All pending processes (web request, loops) are completed - no new processes should start and no new web requests should be accepted.
- Closing all open connections to external services and databases.

There are a couple of things we must figure out in order to shutdown gracefully:

- **When should we shutdown** _-_ Are all pending processes completed, and how we can know this? What if a processes is stuck?
- **How we communicate with processes** - The previous task requires some kind of communication. This is especially true if we are building a modern, asynchronous, and highly concurrent application. So, how can we tell them to shutdown and also know when they've done that?
## Anti-patterns

### Block artificially

The first anti-pattern is the idea of blocking the main go routine without actually waiting on anything. Here's an example toy implementation:
```
func KeepProcessAlive() {
	var ch chan int
	<-ch
}

func main() {
	...
	KeepProcessAlive()
}
```

### os.Exit()

Calling `os.Exit(1)` while other go routines are still running is essentially equal to SIGKILL, no chance for closing open connections and finishing inflight requests and processing.
```
go func() {
		<-ch		
		os.Exit(1)
}()

go func () {

	for ... {

	}
}()
```


## How to make it graceful in Go

In order to gracefully shutdown a service there are two things you need to understand:

1. How to wait for all the running go routines to exit
2. How to propagate the termination signal to multiple go routines

Go provides all the tools we need to properly implement (1) and (2). Let's take a look at these in more detail.

### Wait for go-routines to finish

Go provides sufficient ways for controlling concurrency. Let's see what options are available on waiting go routines.

#### Using channel

Simplest solution, using channel primitive.

1. We create an empty struct channel `make(chan struct{}, 1)` (empty struct requires no memory).
2. Every child go routine should **publish to the channel when it is done** (defer can be useful here).
3. The parent go routine should **consume from the channel as many times as the expected go routines**.

The example can clear things up:
```go
func run(ctx) {
  wait := make(chan struct{}, 1)

	go func() {
		defer func() {
		    wait <- struct{}{}
		}()
		for {
			select {
		    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	}()

	go func() {
		defer func() {
		    wait <- struct{}{}
		}()
		for {
			select {
		    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

	// wait for two goroutines to finish
	<-wait
	<-wait

	fmt.Println("Main done")
}
```


#### With WaitGroup

The channel solution can be a bit ugly, especially with multiple go routines.

[sync.WaitGroup](https://pkg.go.dev/sync#WaitGroup/) is a standard library package, that can be used as a more idiomatic way to achieve the above.

You can also see another [example of waitgroups](https://gobyexample.com/waitgroups/) in use.

```go
func run(ctx) {
	var wg sync.WaitGroup

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				return;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	}()

  wg.Add(1)
	go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				return;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

  wg.Wait()
	fmt.Println("Main done")
}
```

#### With errgroup

The [`sync/errgroup`](https://pkg.go.dev/golang.org/x/sync/errgroup/) package exposes a better way to do this.

- The two `errgroup`'s methods `.Go` and `.Wait` are more readable and easier to maintain in comparison to `WaitGroup`.
- In addition, as its name suggests it does error propagation and cancels the context in order to terminate the other go-routines in case of an error.

```go
func run(ctx) {
	g, gCtx := errgroup.WithContext(ctx)
	g.Go(func() error {
		for {
			select {
		    case <-gCtx.Done():
				fmt.Println("Break the loop")
				return nil;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	})

	g.Go(func() error {
		for {
			select {
		    case <-gCtx.Done():
				fmt.Println("Break the loop")
				return nil;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

  err := g.Wait()
	if err != nil {
		fmt.Println("Error group: ", err)
	}
	fmt.Println("Main done")
}
```

## Terminating a process

Even if we have figured out how to properly communicate the state of processes and wait for them, we still have to implement termination. Let's see how this can be done with a simple example, introducing all the necessary Go primitives.

Let's start with a very simple "Hello in a loop" example:
```go
func main() {
	for {
		time.Sleep(1 * time.Second)
		fmt.Println("Hello in a loop")
	}
}
```

### Introducing signal handling

Listen for an OS signal to stop the progress:
```go
exit := make(chan os.Signal, 1) // we need to reserve to buffer size 1, so the notifier are not blocked
signal.Notify(exit, os.Interrupt, syscall.SIGTERM)
```
- We need to use `os.Interrupt` to gracefully shutdown on `Ctrl+C` which is **SIGINT**
- `syscall.**SIGTERM**` is the usual signal for termination and the default one (it can be [modified](https://docs.docker.com/engine/reference/builder/#stopsignal/)) for [docker](https://docs.docker.com/engine/reference/commandline/stop/) containers, which is also used by [kubernetes](https://kubernetes.io/docs/concepts/workloads/pods/pod-lifecycle/#pod-termination/).
- Read more about `signal` in the [package documentation](https://pkg.go.dev/os/signal/) and [go by example](https://gobyexample.com/signals/).

### Breaking the loop

Now that we have a way to capture signals, we need to find a way to interrupt the loop.

#### Non-Blocking Channel Select

`select` gives you the ability to consume from multiple channels in each `case`.
Our simple hello for loop, now stops on termination signal:
```go
func main() {
	c := make(chan os.Signal, 1) // we need to reserve to buffer size 1, so the notifier are not blocked
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	
	for {
		select {
    case <-c:
			fmt.Println("Break the loop")
			return;
		case <-time.After(1 * time.Second):
			fmt.Println("Hello in a loop")
		}
	}
}
```

### How to do it using Context

Context is a very useful interface in go, that should be used and propagated in all blocking functions. It enables the propagation of cancelation throughout the program.

It is considered good practice for `ctx context.Context` to be the first argument in every method or function that is used directly or indirectly for external dependencies.

### Channel sharing issue

Let's examine how context properties could help in a more complex situation.

_Having multiple loops running in parallel, using channels (counter-example):_

```
// COUNTER EXAMPLE, DO NOT USE THIS CODE
func main() {
	exit := make(chan os.Signal, 1)
	signal.Notify(exit, os.Interrupt, syscall.SIGTERM)
	
  // This will not work as expected!!
	var wg sync.WaitGroup

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-exit: // Only one go routine will get the termination signal
				fmt.Println("Break the loop: hello")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	}()

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-exit: // Only one go routine will get the termination signal
				fmt.Println("Break the loop: ciao")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

	wg.Wait()
	fmt.Println("Main done")
}
```
_Why is this not going to work?_

Go channels do not work in a **broadcast** way, only one go routine will receive a single `os.Signal`. Also, there is no guarantee which go routine will receive it.

`wait := make(chan struct{}{}, 2)`

Context can help us make the above work, let's see how.

#### Using Context for termination

Let's try to fix this problem by introducing [`context.WithCancel`](https://pkg.go.dev/context#WithCancel/)

```go
func main() {
  ctx, cancel := context.WithCancel(context.Background())

	go func() {
		exit := make(chan os.Signal, 1)
		signal.Notify(exit, os.Interrupt, syscall.SIGTERM)
		cancel()
	}()

	var wg sync.WaitGroup

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	}()

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

	wg.Wait()
	fmt.Println("Main done")
}
```
Essentially the `cancel()` is broadcasted to all the go-routines that call `.Done()`.

_The returned context's Done channel is closed when the returned cancel function is called or when the parent context's Done channel is closed, whichever happens first._

### NotifyContext

In go 1.16 a new helpful method was introduced in signal package, [singal.NotifyContext](https://pkg.go.dev/os/signal#NotifyContext/):
```go
func NotifyContext(parent context.Context, signals ...os.Signal) (ctx context.Context, stop context.CancelFunc)
```
_NotifyContext returns a copy of the parent context that is marked done (its Done channel is closed) when one of the listed signals arrives, when the returned stop function is called, or when the parent context's Done channel is closed, whichever happens first._

Using NotifyContext can simplify the example above to:
```go
func main() {
  ctx, stop := context.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
  defer stop()

	var wg sync.WaitGroup

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Hello in a loop")
			}
		}
	}()

	wg.Add(1)
  go func() {
		defer wg.Done()
		for {
			select {
	    case <-ctx.Done():
				fmt.Println("Break the loop")
				break;
			case <-time.After(1 * time.Second):
				fmt.Println("Ciao in a loop")
			}
		}
	}()

	wg.Wait()
	fmt.Println("Main done")
}
```

## Common libraries

### HTTP server

The examples above included a `for` loop for simplification, but let's examine something more practical.

During a non-graceful shutdown, inflight HTTP requests could face the following issues:

- They never get a response back, so they timeout.
- Some progress has been made, but it is interrupted halfway, causing a waste of resources or data inconsistencies if transactions are not used properly.
- A connection to an external dependency is closed by another go routine, so the request can not progress further.

_⚠️ **Having your HTTP server shutting down gracefully is really important.** In a cloud-native environment services/pods shutdown multiple times within a day either for autoscaling, applying a configuration, or deploying a new version of a service. Thus, the impact of interrupted or timeout requests can be significant in the service's SLAs._

Fortunately, go provides a way to gracefully shutdown an HTTP server.

Let us see how it's done:
```go
func main() {

	ctx, cancel := context.WithCancel(context.Background())

	go func() {
		c := make(chan os.Signal, 1) // we need to reserve to buffer size 1, so the notifier are not blocked
		signal.Notify(c, os.Interrupt, syscall.SIGTERM)

		<-c
		cancel()
	}()

	db, err := repo.SetupPostgresDB(ctx, getConfig("DB_DSN", "root@tcp(127.0.0.1:3306)/service"))
	if err != nil {
		panic(err)
	}

	httpServer := &http.Server{
		Addr:    ":8000",
	}

	g, gCtx := errgroup.WithContext(ctx)
	g.Go(func() error {
		return httpServer.ListenAndServe()
	})
	g.Go(func() error {
		<-gCtx.Done()
		return httpServer.Shutdown(context.Background())
	})

	if err := g.Wait(); err != nil {
		fmt.Printf("exit reason: %s \n", err)
	}
}
```
We are using two go routines:

1. run **`httpServer.ListenAndServe()`** as usual
2. wait for `<-gCtx.Done()` and then call **`httpServer.Shutdown(context.Background())`**

It is important to read the package documentation in order to understand how this works:
Shutdown gracefully shuts down the server **without interrupting any active connections**.

Nice, but how?

Shutdown works by first closing all open listeners, then closing all idle connections, and then waiting indefinitely for connections to return to idle and then shut down.

Why do I have to provide a context?

If the provided context expires before the shutdown is complete, Shutdown returns the context's error, otherwise it returns any error returned from closing the Server's underlying Listener(s).

In the example, we chose to provide **`context.Background()`** which has no expiration.

#### Canceling long running requests

When `.Shutdown` is method is called the serve stop accepting new connections and it waits for existing once to finish before `.ListenAndServe()` may return.

There are cases where http requests require quite a long time to be terminated. That could be a for instance a long running job or a websocket connection.

So, what is the best way to terminate those gracefully and not hang waiting for them to finish?

The answer comes into two parts:

1. First of all you need to extract the context from http.Request `ctx := req.Context()` and use this context to terminate your long running process.
2. Use [BaseContext](https://pkg.go.dev/net/http#Server/) (introduced in go1.13), to pass your main ctx as the context in every request

BaseContext optionally specifies a function that returns the base context for incoming requests on this server.  
The provided Listener is the specific Listener that's about to start accepting requests.  
If BaseContext is nil, the default is context.Background().  
If non-nil, it must return a non-nil context.

In the example bellow, a dummy http handler keeps printing in stdout `Hello in a loop`, it will stop either when the request is canceled or the instance receives a termination signal.

```go
func main() {
	mainCtx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()

	httpServer := &http.Server{
		Addr: ":8000",
		Handler: http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			ctx := r.Context()

			for {
				select {
				case <-ctx.Done():
					fmt.Println("Graceful handler exit")
					w.WriteHeader(http.StatusOK)
					return
				case <-time.After(1 * time.Second):
					fmt.Println("Hello in a loop")
				}
			}
		}),
		BaseContext: func(_ net.Listener) context.Context {
			return mainCtx
		},
	}
	g, gCtx := errgroup.WithContext(mainCtx)
	g.Go(func() error {
		return httpServer.ListenAndServe()
	})
	g.Go(func() error {
		<-gCtx.Done()
		return httpServer.Shutdown(context.Background())
	})

	if err := g.Wait(); err != nil {
		fmt.Printf("exit reason: %s \n", err)
	}
}
```

### HTTP Client

Go standard libraries provides a way to pass a context when making an HTTP request: [NewRequestWithContext](https://pkg.go.dev/net/http#NewRequestWithContext/)

Let's see how the following code can be refactored to use it:
```go
resp, err := netClient.Post(uri, "application/json; charset=utf-8",
					bytes.NewBuffer(payload))
...
```
The equivalent with passing ctx:
```go
req, err := http.NewRequestWithContext(ctx, "POST", uri, bytes.NewBuffer(payload))
if err != nil {
			return err
}
req.Header.Set("Content-Type", "application/json; charset=utf-8")

resp, err := netClient.Do(req)
...
```
The following techniques are necessary for more advanced use cases. For instance, if you are using a pool of workers or you have a chain of component dependencies that need to shutdown in order.

### Draining Worker Channels

When you have worker go routines that are consuming/producing from/to a channel, special care must be taken to make sure no items are left in the channels when the process shuts down. To do this we need to utilize go `close` method on the channel. Here's a great overview on [closing channels](https://gobyexample.com/closing-channels/), and a more [advanced article](https://go101.org/article/channel-closing.html/) on the topic.

Two things to remember about closing a channel:

- Writing to a close channel will result in a panic
- When reading for a channel, you can use `value, ok <- ch` . Reading from a close channel will return all the buffered items. Once the buffer items are "drained", the channel will return zero `value` and `ok` will be false. _Note: While the channel still has items `ok` will be true._
- Alternative you can do a `range` on the channel `for value := range ch {` . In this case the for loop will stop when no more items are left on the channel and the channel is closed. This is much prettier than the approach above, but not always possible.

The points above conclude to the following:

- If you have a **single worker writing to the channel**, close the channel once you are done:

```go
go func() {
    defer close(ch) // close after write is no longer possible
    for {
			select { 
				case <-ctx.Done():
					return
				...
        ch <- value // write to the channel only happens inside the loop
    }
}()
```

- If you have **multiple workers writing to the same channel**, close the channel after waiting for all workers to finish:
```go
g, gCtx := errgroup.WithContext(ctx)
ch = make(...) // channel will be written from multiple workers 
for w := range workers { // create n number of workers
  g.Go(func() error {
		return w.Run(ctx, ch) // workers will publish 
	})
}
g.Wait() // we need to wait for all workers to stop
close(ch) // and then close the channel
```
- If you're reading from a channel, exit only when the channel has no more data. Essentially it's the responsibility of the writer to stop the readers, by closing the channel:
```go
for v := range ch {

}

// or
for {
   select {
      case v, ok <- ch:
         if !ok { // nothing left to read
           return;
         }
				 foo(v) // process `v` normally
      case ...:
					...
   }
}
```

- If a worker is both reading and writing, the worker should stop when the channel that it is reading from has no more data, and then close the writer.

## Graceful methods

We have seen several techniques so far for gracefully terminating a piece of long running code. It is also useful to examine how components can expose exported methods that can be called and then facilitate gracefully shutdown.

### Blocking with ctx

This is the most common approach and the easier to understand and implement.

- You call a method
- You pass it a context
- The method blocks
- It returns in case of an error or when context is cancelled / timeout.

```go
// calling:
err := srv.Run(ctx, ...)

// implementation

func (srv *Service) Run(ctx context.Context, ...) {
	...
	...

	for {
		...
    select {
			case <- ctx.Done()
				return ctx.Err() // Depending on our business logic, 
												 //   we may or may not want to return a ctx error:
												 //   https://pkg.go.dev/context#pkg-variables
		 }
	}
```

### Setup/Shutdown

There are cases when blocking with ctx code is not the best approach. This is the case when we want greater control over when `.Shutdown` happens. This approach is a bit more complex and there is also the danger of people forgetting to call `.Shutdown`.

#### Use case

The code bellow demonstrates why this pattern might be useful. We want to make sure that db Shutdown happens only after the Service is no longer running, because the Service is depending on the database to run for it to work.

By calling `db.Shutdown()` on defer, we ensure it runs after `g.Wait` returns:
```go
// calling:
func () {
   err := db.Setup() // will not block
   defer db.Shutdown()

   svc := Service{
     DB: db
   }

   g.Run(...
     svc.Run(ctx, ...)
   )
   g.Wait()
}
```

#### Implementation example
```go
type Database struct {
  ...
	cancel func() 
  wait func() err 
}

func (db *Database) Setup() {
	// ...
	// ...

  ctx, cancel := context.WithCancel(context.Background())
	g, gCtx := errgroup.WithContext(ctx)

  db.cancel = cancel
  db.wait = g.Wait

	for {
		...
    select {
			case <- ctx.Done()
				return ctx.Err() // Depending on our business logic, 
												 //   we may or may not want to return a ctx error:
												 //   https://pkg.go.dev/context#pkg-variables
		 }
	}
}

func (db *Database) Shutdown() error {
	db.cancel()
	return db.wait()
}
```

## Final Thoughts

Terminating your long-running services gracefully is an important pattern that you will have to implement sooner or later. This is especially true for systems like RudderStack that act as middlewares where many connections to external services exist and high volumes of data are handled concurrently.