import foo as bar
import mod.bar as foo

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int
}

fun abaxa(){}

var color: Color=1

interface Foo {
	static var size = 0
	public fun build() {
		self.bar(Color.RED)
	}
}

abstract class A : Foo {
	abstract fun f()
}

fun main() { 
	print("hello, world! my name is aashita")
}