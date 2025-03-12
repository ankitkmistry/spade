import foo as bar
import bar as foo

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
}

interface Foo {
	# var size = 0
	# public init() {}
	public fun bar(color: Color) {}
}

fun main() { 
	var init_fn = Foo.init
	var foo = init_fn()
	foo.bar()
}