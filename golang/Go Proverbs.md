- Don't communicate by sharing memory, share memory by communicating.
- Concurrency is not parallelism.
- Channels orchestrate; mutexes serialize.
- The bigger the interface, the weaker the abstraction.
- Make the zero value useful.
- `interface{}` says nothing.
- Gofmt's style is no one's favorite, yet gofmt is everyone's favorite.
- A little copying is better than a little dependency.
- Syscall must always be guarded with build tags.
- Cgo must always be guarded with build tags.
- Cgo is not Go.
- With the unsafe package there are no guarantees.
- Clear is better than clever.
- Reflection is never clear.
- Errors are values.
- Don't just check errors, handle them gracefully.
- Design the architecture, name the components, document the details.
- Documentation is for users.
- Don't panic.

Author: Rob Pike See more: [https://go-proverbs.github.io/](https://go-proverbs.github.io/)

and... 
### The Zen of Go

- Each package fulfils a single purpose
- Handle errors explicitly
- Return early rather than nesting deeply
- Leave concurrency to the caller
- Before you launch a goroutine, know when it will stop
- Avoid package level state
- Simplicity matters
- Write tests to lock in the behaviour of your package’s API
- If you think it’s slow, first prove it with a benchmark
- Moderation is a virtue
- Maintainability counts

Author: Dave Cheney See more: [https://the-zen-of-go.netlify.com/](https://the-zen-of-go.netlify.com/)