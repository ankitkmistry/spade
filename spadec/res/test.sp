# import foo as bar
# import mod.bar as foo
import lib
import io.print

public enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
}

# You cannot do this
# class A : B {}
# class B : C {}
# class C : A {}

abstract class A {
	public fun f(){}
	public abstract fun b(a:Enum)->int
}
interface B {
	public fun b(a:Enum)->int
}
public class C : A, B {
	public override fun f() {}
	public fun b(a:Enum)->int = 0
}

class CounterFn {
	private var i = 0

	public init() {}

	public fun __call__() -> int {
		i += 1
		return i - 1
	}
}

const do_count = CounterFn()

fun lambda() {
	const fn1 = fun (i: int) { return i + 1 }
	const fn2 = fun (x: int, y: int): x + y
	const fn3 = fun (): 124124
	const fn4 = fun : 124124
	const fn5 = fun -> int: 124124
	const fn6 = fun { 124124 }

	fn1(1)
	fn2(35, 34) as void
	fn3() as void
	fn4() as float as void
	fn5() as void
	fn6()

	# fn()

	# var a : A
	# a?.f()

	# var fn = fn1 as int
	# fn1(1)
}

class Kacha {
	init() {}
	public fun get_aam() {}
	public fun get_mango() {}
	public fun get_amro() {}
	public fun get_ammo() {}
}

public fun kacha_test() {
	var kacha: Kacha = Kacha()
	# kacha.get_amo() # show a beautiful error message
	{}
	if true {
		kacha = Kacha()
		kacha.get_aam()
	} else {
		if true {
			kacha = Kacha()
			kacha.get_aam()
		} else {
			kacha.get_amro()
		}
	}
}

public fun main() { 
	var text = "hello, world"
	text = text[::-1] # "dlrow ,olleh"

	lambda()
	
	var str_buf: lib.StringBuilder? = lib.StringBuilder("west ")
	print(do_count(), do_count(), do_count(), sep: ", ")
	str_buf?[0] = " "
	while false {} # TODO: do something about line separators!!
	str_buf?[1] = " "
	while false {}
	(str_buf ??= lib.StringBuilder()) += lib.StringBuilder("bengal\n")
	print(lib.StringBuilder()
		.appendln("hello")
		.appendln("this is from spade")
		.appendln("this text is built from lib.StringBuilder")
		.keep(0, 5)
		.clear()
		.to_string()
	)
	print((0).to_string())
	print("hello, world! my name is aashita", end: "")
	print()
}