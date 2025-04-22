import foo as bar
import mod.bar as foo

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

fun abaxify(parg1:int,*,ac:string){}
fun abaxify(a:int,b:int,c="1"){}
fun abaxify(a,/,b:string,c="a",*args){}

var abaxa=abaxify(a:1,b:2)

class Foo {
	static var size = build()
	public static fun build() -> int {
	}
}

const diddy = Foo.build()

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

var b_obj: B?
# var test_obj = b_obj?.b(Color.RED) 
var test_obj = b_obj?.b?(Color.RED) 

fun print(data:string=""){
	# test_obj()
	# test_obj?()

	# while true:
	# 	while false:
	# 		continue
	# 	else:
	# 		break
	# else {
	# 	return
	# 	while false:
	# 		continue
	# 	else:
	# 		continue
	# 	# break
	# }
	
	return
}

fun main() { 
	print("hello, world! my name is aashita")
	print()
}