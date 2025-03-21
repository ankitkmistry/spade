import foo as bar
import mod.bar as foo

var global1 = Aa.a as B # no error here but no more

class Aa{
	var a
}

class B{
	class a
	var b: B
}

# var global2 = Aa.a as B # but error here!!!

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

var a: B?
var b: any? = a?.b

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