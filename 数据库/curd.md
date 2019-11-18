## 预备知识

### 别名

在一个列名或表名后加`AS 别名`或省略AS

```mysql
SELECT name AS n
FROM   class AS c;
```



### 去重

在SELECT中加`DISTINCT`表示去重

```mysql
SELECT DISTINCT age
FROM   class;
```



### 字符串连接

`concat(str1, str2, ..., strN)`



### 模糊关键字

LIKE， 后跟一个字符串，字符串中%表示任意个数字符，_表示任意一个字符。

```mysql
SELECT name
FROM class
WHERE name LIKE '%i%';
```

### 字符函数

length, concat, substr, instr, trim, upper, lower, lpad, rpad, replace



### 数学函数

round，ceil,  floor,  truncate,  mod,  



### 日期函数

str_to_data, data_format, now, curdata, curtime



### 控制函数

```mysql
case colume 
    when condition then result
    when condition then result
    when condition then result
else result
end  
```

```mysql
#如果 expr1 是TRUE (expr1 <> 0 and expr1 <> NULL)，则 IF()的返回值为expr2; 否则返回值则为 expr3。
IF(expr1,expr2,expr3)
#假如expr1 不为 NULL，则 IFNULL() 的返回值为 expr1; 否则其返回值为 expr2。
IFNULL(expr1,expr2)
```



### 分组函数/聚合函数

| 函数  | 意义   |
| :---- | ------ |
| avg   | 平均值 |
| sum   | 求和   |
| max   | 最大值 |
| min   | 最小值 |
| count | 计数   |

可以与distinct搭配用于去重，会自动忽略NULL

## 查

```mysql
# 加入连接后
SELECT 【DISTINCT】 列名1 【AS】别名1，列名2 【AS】别名2，...，列名n 【AS】别名n
FROM 表1 【AS】别名1
【INNER/LEFT/RIGHT/FULL】JOIN 表2 【AS】别名2
ON   连接条件
WHERE 筛选条件
GROUP BY 分组根据（可以多个）
HAVING   组筛选条件
ORDER BY 排序根据 【DESC, ESC】
LIMIT 显示数量 ;

```

顺序：

- FROM 
- JOIN
- ON 
- WHERE
- GROUP BY `(从此处开始可以使用select中指定的别名)`
- AVG、SUM、MAX
- HAVING
- SELECT
- DISTINCT
- ORDER BY

通过**FROM**和**JOIN**以及**ON**形成新视图，然后现在视图上通过**WHERE**筛选，然后通过**GROUP BY**分组，然后对每组计算**聚合函数**，再通过**HAVING**筛选组，再SELECT提前需要的信息，然后DISTINCT去重，ORDER BY排序.

## 增

```mysql
INSERT INTO 表名【(列名1， 列名2，....,列名n)】 VALUE(值1， 值2， ...,值n), (值1， 值2， ...,值n)...；

INSERT INTO 表名 SET 列名1 = 值1, ..., 列名n = 值n;
```



## 删

```mysql
DELETE FROM 表名 WHERE 筛选条件;
TRUNCATE table 表名; # 删空
```



## 改

```mysql
UPDATE 表名
SET 列1 = 值1, ..., 列n = 值n
#通过连接可以同时修改多个表
【inner|left|right join 表2 别名
  on 连接条件】
WHERE 筛选条件;
```

