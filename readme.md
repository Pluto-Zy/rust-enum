# Implementing the enum of Rust in C++

The [enum](https://doc.rust-lang.org/book/ch06-01-defining-an-enum.html) is a fantastic feature of Rust. It is similar to a tagged union (just like `std::variant<>` in C++), but Rust provides a lot of optimizations for enums.

In this project, I will try to implement Rust's enum in C++. I will implement my own `variant` step by step and apply some optimizations. Of course, I can't apply all the optimizations Rust does to enum to my `variant`, because some of them require help from the compiler.

My steps are as follows:

1. Implement `std::variant<>` introduced in C++17, but without too many optimizations.
2. Optimize for fundamental types and reference types.
3. Optimize for class types and tuple.
4. Optimize for `variant<>` types.
5. Try implementing it with C++11 or C++14.
6. ...