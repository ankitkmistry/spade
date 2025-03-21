import foo as bar
import mod.bar as foo

# var global = Aa.a as B

class Aa{
	var a
}

class B{
	class a
	var b
}


enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

fun abaxa(){}

interface Foo {
	static var size = 0
	public fun build() {
		self.bar(Color.RED)
	}
}

abstract class A : Foo {
	var color: Color=Color.RED
	abstract fun f()
}

fun main() { 
	print("hello, world! my name is aashita")
}