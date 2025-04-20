import foo as bar
import mod.bar as foo

# var global1 = Aa.a as B # no error here but no more

class Aa{
	var a: int = 5
}

# var global2 = Aa.a as B # but error here!!!

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

# var a: B?

fun abaxify(parg1:int,*,ac:string){}
fun abaxify(a:int,b:int,c="1"){}
fun abaxify(a,/,b:string,c="a",*args){}

var abaxa=abaxify(a:1,b:2)

class Foo {
	static var size = build()
	public static fun build() -> int {
		self.bar(Color.RED)
	}
}

const diddy = Foo.build()

# abstract class A : Foo {
# 	var color: Color=Color.RED
# 	init(){}
# 	abstract fun f()
# }

# You cannot do this
# class A : B {}
# class B : C {}
# class C : A {}

class A {
	abstract fun b(a:Enum)->int
}
interface B {
	fun b(a:Enum)->int
}
class C : A, B {}

var b_obj: B
var test_obj = b_obj.b(Color.RED) 

fun main() { 
	print("hello, world! my name is aashita")
}