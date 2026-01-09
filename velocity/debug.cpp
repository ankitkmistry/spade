void DebugOp::print_const_pool(const vector<Obj *> &pool) {
    if (pool.empty())
        return;
    auto max = std::to_string(pool.size() - 1).length();
    std::cout << "Constant Pool\n";
    std::cout << "-------------\n";
    for (size_t i = 0; i < pool.size(); ++i) {
        const auto obj = pool.at(i);
        string type_str;
        if (obj->get_type()) {
            type_str = obj->get_type()->to_string();
        } else if (is<ObjNull>(obj))
            type_str = "<null>";
        else if (is<ObjBool>(obj))
            type_str = "<basic.bool>";
        else if (is<ObjChar>(obj))
            type_str = "<basic.char>";
        else if (is<ObjInt>(obj))
            type_str = "<basic.int>";
        else if (is<ObjFloat>(obj))
            type_str = "<basic.float>";
        else if (is<ObjString>(obj))
            type_str = "<basic.string>";
        else if (is<ObjArray>(obj))
            type_str = "<basic.Array>";
        else
            throw Unreachable();
        std::cout << std::format(" {: >{}d}: {} {}\n", i, max, type_str, obj->to_string());
    }
}
