import foo as bar
import bar as foo

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int
}

interface Foo {
	static var size = 0
	public fun build() {
		self.bar(Color.RED)
	}
	public final static fun bar(color: Color)
}

abstract class A : Foo {
	abstract fun f()
}

fun main() { 
	var init_fn = Foo.init
	var foo = init_fn()
	foo.bar()
}