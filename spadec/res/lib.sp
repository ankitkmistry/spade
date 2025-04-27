public class StringBuilder {
	private var data:string = ""

	public init(data: string? = "") {
		self.data = data ?? ""
	}

    public fun clear() -> StringBuilder {
        self.data = ""
        return self
    }

	public fun append(*datas:string?) -> StringBuilder {
		# TODO: To be changed
		self.data += datas ?? ""
		return self
	}

	public fun appendln(*datas:string?) -> StringBuilder {
		# TODO: To be changed
		self.data += datas ?? "" + "\n"
		return self
	}

    public fun reverse() -> StringBuilder {
        var new_data = ""
        var i = 0
        while i < self.size() {
            new_data = new_data + self.__get_item__(i)
            i += 1
        }
        self.data = new_data
        return self
    }

    public fun substring(start: int, end: int) -> StringBuilder {
        var data = ""
        var i = 0
        while i < self.size() {
            if start <= i < end: data += self.__get_item__(i)
            i += 1
        }
        return StringBuilder(data)
    }
    
    public fun substring(start: int) -> StringBuilder = self.substring(start, self.size())

    public fun keep(start: int, end: int) -> StringBuilder {
        var data = ""
        var i = 0
        while i < self.size() {
            if start <= i < end: data += self.__get_item__(i)
            i += 1
        }
        self.data = data
        return self
    }

    public fun keep(start: int) -> StringBuilder = self.keep(start, self.size())

	public fun __add__(other:StringBuilder?) -> StringBuilder {
		return init(self.data + other?.data ?? "")
	}

	public fun __aug_add__(other:StringBuilder?) -> StringBuilder {
		self.data += other?.data ?? ""
		return self
	}

    public fun __get_item__(index: int) -> string = ""

    public fun size() -> int = 0
    public fun capacity() -> int = 0
	public fun as_string() -> string = self.data
	public fun to_string() -> string = self.data
}