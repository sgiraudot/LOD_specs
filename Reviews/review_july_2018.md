# Review July 2018

## Remarks

 * Use american English (Color instead of Colour, Neighbor instead of
   Neighbour, Barycenter instead of Barycentre, etc.)
 
 * Never use `std::list` if you don't have a good reason to use it
   (usually, that means if you need to do many deletions or to splice
   the content among different lists). Otherwise always use
   `std::vector` which is much more efficient (and also has a more
   flexible API with the `operator[]`).
   
 * Please set-up your text editor so that it always uses _spaces_ for
   indentation and not tabulations (tabs are not displayed the same
   everywhere and that often screws up how the code is displayed). Two
   spaces per tab should be sufficient (see
   <https://doc.cgal.org/latest/Manual/devman_code_format.html>).
 
 * Also, avoid lines that are too long (the usual way is to never go
   over 80 characters, but that's probably a bit outdated,
   nevertheless you shouldn't go over 100)
   
 * Always pass property maps _by copy_, not with refs or const refs
   (that leads to problems if user passes temporary objects, which
   happens a lot with property maps). Property maps are documented as
   lightweight objects, so it's not a problem to copy them. Property
   maps are supposed to be mostly "glue" between user's structures and
   code's structure. If your property map has 10 methods and
   complicated computations in it, it should probably not be a
   property map (which leads me to the next point).

__Example__: why is `Building_with_segment_constraints_property_map.h` a
property map at all? It seems to me that it is a building tagger that
fills an `std::map` that is later accessed through `put()` to fill the
face `info()->group_number()`. This is both overly complicated and
very inefficient: why not simply call this a building tagger and make
it fill directly the `info()->group_number()`?
   
 * When I was mentionning property maps, I was talking from a public
   API point of view: it's nice that you understand well how it works
   now, but don't feel obligated to put property maps everywhere in
   your code just for the sake of using property maps. If it's only
   used internally with your own structures, then it's not really
   useful and it makes the code more complicated than it could
   be. Internally, keep things simple, use directly your own
   structures (`std::vector`, etc.). An example: there is no interest
   in using an identity map for the `Regularized_segment_map` that no
   user is ever going to see.

__Example__ in `Kinetic_based_partitioning_2.h`:

```c++
  using Internal_point_map   = CGAL::Identity_property_map<Point_2>;
  using Internal_segment_map = CGAL::Identity_property_map<Segment_2>;
```

What's the point? The identity map is provided for the user as the
simplest property map (where the elements passed as parameters are
_exactly_ what should be used by the function). In your case, why not
access the elements directly are these are internal strucures that
_you_ define and that _you_ control?

Similarly,

```c++
using Regularized_segment_map = CGAL::Identity_property_map<Segment_2>;
```

in your data structure. I don't see how this is of any help; it
complicates lots of your code by adding a useless `SegmentMap`
template parameter; and you even have a method in
`Kinetic_based_partitioning_2.h` called `create_segments()` whose only
job is to fill a `std::vector<Segment_2>` (the exact same structure as
you have in your data structure) through this identity property map.
 
 * Again, more generally, keep in mind who you are writing the code
   for: users need flexibility, so they need templates and property
   maps. On the other hand, no user is going to read and change the
   internal code, but developers will. And it's easier if the code is
   clear at first sight, without having to look and understand an
   intrication of (undocumented because internal) property maps,
   templates and functors in subclasses hidden in a 3 folder deep
   hierarchy of files. Do not complicate your code for the sake of
   being overly generic especially if you don't intend to reuse the
   code. For example, use hard-coded types instead of templates if you
   know what the type is and you don't intend to instantiate your code
   with another type; if the `elements` of a parameter are always
   segments, then call them `segments`.
   
 * If you create a class with 0 attribute and just a set of methods,
   then these methods should probably just be free-functions. Note
   that it can make the code easier to read/use as free template
   functions can take advantage of template deduction (which can't be
   done with template classes).
 
 * I've seen several times things like
   `m_big_value(FT(100000000000000))`. Don't do that, use
   `std::numeric_limits<FT>::max()` or
   `std::numeric_limits<FT>::inf()` if you want to represent a number
   larger than all the others.
 
 * `using namespace LOD = CGAL::Level_of_detail` at the beginning of
   each file is useless: your classes are all in
   `CGAL::Level_of_detail`, so you don't need to write this namespace
   (or `LOD`) when you refer to another class.

## Regularizer

I found that in most cases, regularizing was creating more problems
and was making the output worse than before. Did you find cases where
it actually made things better?
 
I find this section quite worrying:
 
```c++
			// these are weak parameters
            m_lambda(FT(4) / FT(5)),        		   
            m_epsilon(FT(1) / FT(4)),       		   
			m_small_fixed_orientation(FT(1) / FT(10)), 
			m_small_fixed_difference(FT(1) / FT(10)),  
			m_tolerance(FT(1) / FT(1000000)),		   
			m_ooqp_problem_weight(FT(100000)),		   
```

These seem like a lot of magic parameters and I seriously doubt that they
will match every case.

The regularizer in general seems like a big complicated machinery that
provides really random results (unless I just use it wrong). In
addition, it depends on this OOQP solver whose license in not
compatible with CGAL.

Do we really need this regularizer? It seems to me that very local and
naive regularization (just greedily apply parallelism and
orthogonality inside each building separately) would produce better
results with less trouble.

