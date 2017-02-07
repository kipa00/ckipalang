ckipalang Documentation
========

ckipalang code is a sequence of kipalang words. A kipalang word is classified into three types: literals, variables, and operators.

### Literals

A literal is a type of constant that will not change throughout program execution. Some examples of literals are <code>3</code>, <code>200000000000</code>, <code>4.5</code> and <code>"a literal"</code>. A literal must be one of these four types:
* int consists only of digits(i.e. numerical characters), and when evaluated, is smaller than <code>2147483648</code>.
* long consists only of digits, and is not int(i.e. when evaluated, is greater than or equal to <code>2147483648</code>).
* float consists only of digits and exactly one period.
* intlist starts and ends with double quotes.

### Variables

A variable is a storage of data. In kipalang codes, a sequence of alphabets and underscore that is not a reserved word for operation will be considered variables.

### Operators

An operator is a tool to manage literals or variables. kipalang currently has 20 operators.

* <code>+</code>, <code>-</code>, <code>*</code>, <code>/</code> are the basic four binary operators.
* <code>+</code> and <code>-</code> work as a positive operator and a negative operator respectively if this is the start of the program or there are no literals or the zero-parameter operator, and are considered different operators from the binary + and -.
* <code>print</code> and <code>putch</code> are called the output operator.
  * <code>print</code> prints literals or variables' value in the form of getting these values in kipalang. this means that if you put into your kipalang code these output values, you can get the same value literals or variables have. Note that when <code>print</code>ing float values, there might be trailing zeroes.
  * <code>putch</code> prints literals or variables' value that is either a type of int or intlist in the form of character that has their respective value.
* <code>==</code>, <code>&lt;</code>, <code>&gt;</code> are the comparison operator.
* <code>,</code> and <code>;</code> are called the concatenation operator.
  * <code>,</code> gets the two values and concatenate to make a list.
  * <code>;</code> simply ignores the left value and returns the right value. this works as a statement concatenation operator.
* <code>=</code> is the substitute operator.
* <code>if</code> and <code>while</code> are called the branch operator.
  * <code>if &lt;condition&gt; &lt;statement&gt;</code> executes <code>&lt;statement&gt;</code> if <code>&lt;condition&gt;</code> is not zero(int zero, long zero, float zero and null pointer all work as zeroes). if returns the execution result of <code>&lt;statement&gt;</code> if <code>&lt;condition&gt;</code> is not zero, or simply <code>0</code>.
  * <code>while &lt;condition&gt; &lt;statement&gt;</code> executes <code>&lt;statement&gt;</code> as long as <code>&lt;condition&gt;</code> is not zero. <code>while</code> returns the last execution result of <code>&lt;statement&gt;</code>, or <code>0</code> if <code>&lt;statement&gt;</code> has not been executed at least once.
* <code>in</code> is called the loop operator. <code>&lt;statement&gt; &lt;variable&gt; in &lt;list&gt;</code> executes <code>&lt;statement&gt;</code> as if the referred variable has the value sequentially the element in list.
* <code>range &lt;value&gt;</code> makes the <code>0</code> through <code>&lt;value&gt;-1</code> list.
* <code>len</code> returns the length of a list. if the parameter is not a list, then returns <code>1</code>.
* <code>scanint</code> scans from stdin an int. it is zero-parameter operator, meaning it must have no parameters.
* <code>&lt;list&gt;[&lt;index&gt;]</code> gets the value at <code>&lt;index&gt;</code> from <code>&lt;list&gt;</code>. indexes start from <code>0</code>.

### Operator Priority

Operators have their own priority. The former the operator is listed, the faster it is evaluated.

* <code>len</code>, <code>range</code>, <code>scanint</code>
* <code>[]</code>
* the unary <code>+</code>, <code>-</code>
* <code>*</code>, <code>/</code>
* the binary <code>+</code>, <code>-</code>
* <code>==</code>, <code>&lt;</code>, <code>&gt;</code>
* <code>print</code>, <code>putch</code>
* <code>,</code>
* <code>=</code>
* <code>if</code>, <code>while</code>
* <code>in</code>
* <code>;</code>

note that using <code>(</code> and <code>)</code> you can make some operators have higher priority than other operators, regardless of what the operator is.
