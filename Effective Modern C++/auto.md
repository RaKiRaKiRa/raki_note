### 对于局部变量类型，需要加typename

```cpp
template<typename T>
void dwim(It b, It e)
{
    while(b!=e){
        typename std::iterator_traits<It>::value_type
        currValue = *b;
    }
}
```

`std::iterator_traits::value_type `是`std::iterator_traits<It>`对象的一个局部变量类型，然而为了让编译器知道**这是变量类型而不是变量**，在用于声明时需要在前面加上`typename`

