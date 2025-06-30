import io

# Prints fibonacci numbers less than `n`
public fun fib(n: int) {
    var a = 1
    var b = 1
    while b < n {
        print(a)
        a, b = b, a + b
    }
}