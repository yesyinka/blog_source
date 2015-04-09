Title: 99 Scala Problems 10 - Run-length encoding of a list.
Date: 2015-04-xx xx:xx:00 +0100
Category: Programming
Tags: Scala, functional programming
Authors: Leonardo Giordani
Slug: 99-scala-problems-10-run-length-encoding-of-a-list
Series: "99 Scala Problems"
Summary: 

## The problem

**P10** (*) Run-length encoding of a list.
Use the result of problem P09 to implement the so-called run-length encoding data compression method. Consecutive duplicates of elements are encoded as tuples (N, E) where N is the number of duplicates of the element E.

Example:

``` scala
scala> encode(List('a, 'a, 'a, 'a, 'b, 'c, 'c, 'a, 'a, 'd, 'e, 'e, 'e, 'e))
res0: List[(Int, Symbol)] = List((4,'a), (1,'b), (2,'c), (2,'a), (1,'d), (4,'e))
```

## Initial thoughts

The problem explicitly states "use the result of problem 09" ([here](/blog/2015/04/07/99-scala-problems-09-pack-consecutive-duplicates/)) so I think it's time for me to learn how to make and import libraries. The problem itself seems to be rather simple to solve.

## The recursive solution

Given the availability of the `pack()` function from problem 09 the recursive solution just has to walk the packed list and convert each element (a list of equal elements) into a tuple. This is the tail recursive version

``` scala
def encode[A](l: List[A]):List[(Int, A)] = {
    def _encode(res: List[(Int, A)], rem: List[List[A]]):List[(Int, A)] = rem match {
        case Nil => res
        case h::tail => _encode(res:::List((h.length, h.head)), tail)
    }
    _encode(List(), utils.packer.pack(l))
}
```

Note that I use the `utils.packer.pack()` function imported from a package which contains the code developed for problem 09.

