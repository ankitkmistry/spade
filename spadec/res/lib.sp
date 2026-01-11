public class StringBuilder {
	private var data: string = "";

	public init(data: string? = "") {
		data = data ?? "";
	}

    public fun clear() -> StringBuilder {
        data = "";
        return self;
    }

	public fun append(*datas: string?) -> StringBuilder {
		# TODO: To be changed
		data += datas ?? "";
		return self;
	}

	public fun appendln(*datas: string?) -> StringBuilder {
		# TODO: To be changed
		data += datas ?? "" + "\n";
		return self;
	}

    public fun reverse() -> StringBuilder {
        var new_data = "";
        var i = 0;
        while i < size() {
            new_data = new_data + self[i];
            i += 1;
        }
        data = new_data;
        return self;
    }

    public fun substring(start: int, end: int) -> StringBuilder {
        var data = "";
        var i = 0;
        while i < size() {
            if start <= i < end: data += self[i];
            i += 1;
        }
        return StringBuilder(data);
    }
    
    public fun substring(start: int) -> StringBuilder = substring(start, size());

    public fun keep(start: int, end: int) -> StringBuilder {
        var data = "";
        var i = 0;
        while i < size() {
            if start <= i < end: data += self[i];
            i += 1;
        }
        data = data;
        return self;
    }

    public fun keep(start: int) -> StringBuilder = keep(start, size());

	public fun __add__(other: StringBuilder?) -> StringBuilder {
		return init(data + other?.data ?? "");
	}

	public fun __aug_add__(other: StringBuilder?) -> StringBuilder {
		data += other?.data ?? "";
		return self;
	}

    public fun __get_item__(index: int) -> string = data[index];
    public fun __set_item__(index: int, value: string) -> void {}

    public fun size() -> int = 0;
    public fun capacity() -> int = 0;
	public override fun to_string() -> string = self.data;
}
