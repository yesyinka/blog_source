Title: 99 Scala Problems 09 - Pack consecutive duplicates of list elements into sublists
Date: 2015-04-02 23:30:00 +0100
Category: Programming
Tags: Scala, functional programming
Authors: Leonardo Giordani
Slug: 99-scala-problems-09-pack-consecutive-duplicates
Series: "99 Scala Problems"
Summary: 

## The problem

**P09** (**) Pack consecutive duplicates of list elements into sublists. If a list contains repeated elements they should be placed in separate sublists.

Example:

``` scala
scala> pack(List('a, 'a, 'a, 'a, 'b, 'c, 'c, 'a, 'a, 'd, 'e, 'e, 'e, 'e))
res0: List[List[Symbol]] = List(List('a, 'a, 'a, 'a), List('b), List('c, 'c), List('a, 'a), List('d), List('e, 'e, 'e, 'e))
```

## Initial thoughts

This is similar to [problem 08](/blog/2015/04/02/99-scala-problems-08-eliminate-consecutive-duplicates/) but has an important difference: the result will be a list of lists. When dealing with multi-level structures like these, it is very easy to get lost, so I expect to find a recursive solution with a lot of cases or complex pattern guards.

## The recursive solution

The tail recursive solution is easier to write than any other solution. The resulting list we are building is a `List[List[A]]`, while the remainder is just a plain list `List[A]`.

The first case is straightforward. The second case extracts an element from the list and, if the result is empty of the result doesn't contain that element, appends a new list. Pay attention: we have to check `res.last.head` since `res` is a list of lists. So `res.last` is the last list we put into `res` and its head is the repeated element. Obviously the same thing is accomplished by checking `res.last.last`, since this algorithm packs together equal values. The third case covers the situation in which the extracted element is already in the last list or `res`, so we have to modify this latter. To do this we separate `res` in `res.init` and `res.last`, append `h` to `res.last` (`res.last:::List(h)`) and merge again the two pieces.

``` scala
def pack[A](l: List[A]):List[List[A]] = {
    def _pack(res: List[List[A]], rem: List[A]):List[List[A]] = rem match {
        case Nil => res
        case h::tail if (res.isEmpty || res.last.head != h) => _pack(res:::List(List(h)), tail)
        case h::tail => _pack(res.init:::List(res.last:::List(h)), tail)
    }
    _pack(List(),l)
}
```

A very simpler solution, however, comes from the `span()` method of `List` objects.
TODO TODO TODO

``` scala
def pack2[A](l: List[A]):List[List[A]] = {
    def _pack2(res: List[List[A]], rem: List[A]):List[List[A]] = rem match {
        case Nil => res
        case ls => {
            val (s: List[A], r: List[A]) = rem span { _ == rem.head }
            _pack2(res:::List(s), r)
        }
    }
    _pack2(List(), l)
}
```

## Final considerations
## Feedback

Feel free to use [the blog Google+ page](https://plus.google.com/u/0/b/110554719587236016835/110554719587236016835/posts) to comment the post. The [GitHub issues](https://github.com/lgiordani/lgiordani.github.com/issues) page is the best place to submit corrections.

