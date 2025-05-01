public class any {
    public fun __eq__(other: any?) -> bool {
        return self is other if other else false
    }

    public fun __ne__(other: any?) -> bool {
        return not self == other
    }

    public fun to_string() -> string = ""
}

public class Enum {}
public class Annotation {}
public class Throwable {}

public class Exception : Throwable {
    private var message: string

    public init(message: string="") {
        self.message = message
    }

    public final fun get_message() -> string = self.message
}

public class AssertError : Exception {
    public init(message: string="") {
        super.init("Assertion failed: " + message)
    }
}

public final class int {}
public final class float {}
public final class bool {}

public final class string {
    public init(obj: any? = null) {}

    public fun __get_item__(i: int) -> string = ""
    public fun __get_item__(slice: Slice) -> string = ""
    public fun __add__(other: string?) -> string = ""
    public fun __rev_add__(other: string?) -> string = ""
    public fun __aug_add__(other: string?) -> string = ""
}

public final class void {}

public fun assert(condition: bool, message: string="") {
    if not condition: throw AssertError(message)
}

public final class Slice {
    private var start: int?
    private var end: int?
    private var step: int?

    public init(/, start: int?, end: int?, step: int?){
        self.start = start
        self.end = end
        self.step = step
    }

    public fun get_start() -> int? = start
    public fun get_end() -> int? = end
    public fun get_step() -> int? = step
}

public interface Iterable {
    public fun iter() -> Iterator
}

public interface Iterator {
    public fun has_next() -> bool
    public fun next() -> any?
}