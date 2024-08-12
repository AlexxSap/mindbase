Позволяет свернуть 2 вектора в вектор пар

```rust 
fn zip<T1, T2>(v1: Vec<T1>, v2: Vec<T2>) -> Vec<(T1, T2)> {
    v1.into_iter().zip(v2.into_iter()).collect()
}

fn main() {
    let alf = vec![1, 2, 3];
    let num = vec!['a', 'b', 'c'];

    let zipped = zip(alf, num);

    for pair in zipped {
        println!("{:?}", pair);
    }
}
```