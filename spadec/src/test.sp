import foo as bar
import bar as foo

class Foo {
	public init() {}
	public fun bar() {}
}

fun main() { 
	var init_fn = Foo.init
	var foo = init_fn()
	foo.bar()
}