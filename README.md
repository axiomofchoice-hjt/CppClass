# CppClass

随手写的类似 protobuf 的工具

## 枚举类型

定义纯 enum 类型

```text
enum Calc {
    Add,
    Sub,
    Mul,
    Div
}
```

定义携带数据的 enum 类型

```text
enum Result {
    Ok,
    Err(int)
}
```

接口：

- `Calc calc;` 定义一个空的 Calc
- `calc = Calc::Add();` 赋值为 Calc::Add
- `auto res = Result::Err(1);` 定义携带数据的 Err
- `calc == nullptr` 判断是否为空
- `calc.is_Add()` 判断是否为 Add
- `calc == other_calc` 判断两个 Calc 是否相等（只能用于纯 enum 类型）
- `res.get_Err()` 返回携带的数据（类型不安全，请先用 `is_Err()` 判断后再 `get_Err()`）

序列化：

- `CppClass::toBinary(res)` 得到二进制序列化的结果，类型为 `std::vector<uint8_t>`
- `CppClass::fromBinary<Result>(vec)` 得到二进制反序列化的结果，参数类型是 `std::vector<uint8_t>`，结果类型是 `Result`
- `CppClass::toJson(res)` 得到 Json 序列化的结果，类型为 `std::string`
- `CppClass::fromJson<Result>(str)` 得到 Json 反序列化的结果，参数类型是 `std::string`，结果类型是 `Result`