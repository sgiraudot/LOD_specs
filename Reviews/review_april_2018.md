# Review April 2018

## Files and namespaces

* It is a good thing that you put everything in a subnamespace. I
  would change the namespace `LOD` to `Level_of_detail`: in general,
  it's better to use the full names, abreviations should be reserved
  to user code (see `CGAL::Polygon_mesh_processing` that's often used
  with `using PMP = CGAL::Polygon_mesh_processing`).

* You have many many submodules that you put in folders. That's good,
  although these folders should be themselves in a general folder
  `Level_of_detail`, so that we know what the included file is
  about. That way, you could also remove the prefix `Level_of_detail_`
  from all your files. That means that for example, the headers would
  change like this:
  
```c++
- #include <CGAL/Clutter/Level_of_detail_clutter_processor.h
+ #include <CGAL/Level_of_detail/Clutter/Clutter_processor.h
```

* The class `Level_of_detail_base` should be renamed simply
  `Level_of_detail`: this is the core of the algorithm and it will be
  the public interface for the user.

## Separation of file streams, data and algorithms

* LOD is an algorithm and the code should just be about the algorithm:
  handling of file streams or of specific data structures should not
  be part of the `include` files. Keep in mind that we are providing a
  _library_, not an end-user program. So anything related to files,
  structures, parameters, should _not_ appear in your package.

* Although the data structure is a template `Container_3D`, in
  practise your algorithm will only work with `CGAL::Point_set_3`: you
  are using methods such as `input.template
  property_map<int>("label")` which are exactly the methods of
  `CGAL::Point_set_3`. The most common way to handle input data in
  CGAL is through property maps (see
  [the PSP manual](https://doc.cgal.org/latest/Point_set_processing_3/index.html#title1)). Basically,
  if you need for example a label associated to a point, the user
  should be asked to provide a property map that associates the
  `value_type` of the iterator of the input range to the
  label. Something like this:

```c++
  // Current API that assumes PointRange is a CGAL::Point_set_3
  template <typename PointRange>
  void function (const PointRange& input)
  {
    Container_3D::iterator it = input.begin();
    Container_3D::Property_map<int> labels = input.property_map<int>("label").first;
    int label_of_it = labels[*it];
  }
  
  // Wanted API that can deal with any data the user provides
  template <typename PointRange, typename LabelMap>
  void function (const PointRange& input, LabelMap labels)
  {
    Container_3D::iterator it = input.begin();
    int label_of_it = get (labels, *it);
  }
```

* You should never deal with file streams inside the `include` code
  (except if you need specific readers, which you don't). The
  algorithm should take as input the point set with the needed
  property maps (point map, normal map, label map, etc.).

* Your log system should also be completely separated from the code,
  as it is very specific and won't work for the average user. As you
  probably need it for debugging, one option I would recommend is to
  systematically surround your calls to your Log class by something
  like:

```c++
#ifdef CGAL_LOD_SAVE_LOGS
   m_my_log.do_something();
#endif
```

  This way, you just need to define `CGAL_LOD_SAVE_LOGS` as the
  beginning of your test file to get your behavior, but the user won't
  have to worry about it.

* Your system to handle parameters is very specific and should not be
  provided or documented: again, providing ways to deal with the
  `main()` arguments from the command line is totaly out of scope for
  a library's component. Parameters should be provided for their
  related function.

## Template, parameters and scope

* Generally, you have to think in terms of scope and what is the most
  practical from the user's point of view. For example, if a parameter
  is used for a single method inside a very long class, it probably
  doesn't make sense to have it as an attribute of the class (for
  example, `m_graph_cut_alpha`). On the other hand, if a parameter is
  used all over the class, it probably makes sense to have it as an
  attribute of the class (the `Container_3D` object `input` that you
  pass from method to method should definitely be an attribute of the
  class, and should actually be given in the constructor).

* You have a very monolithic traits class that does not seem really
  user friendly. Although I understand it can be practical from a
  developer's point of view, it's not realistic to expect users to
  create a traits class anytime they just want to change one step of
  the algorithm. For example, you have a `Preprocessor` object in the
  traits class: again, in terms of scope, that means that if I want to
  change this processor, I have to change the whole traits class. If
  this object is just used at step 1 out of N, then it should be a
  template parameter of step 1.

* Imagine that I want to create a demo where the user is free to
  select 10 ways to select ground and 10 ways to select building
  boundary: in your current framework, I would have to instanciate 100
  different traits class to do that. It's not realistic: if, on the
  contrary, the template was not in the traits but directly on the
  detection methods, I would just need to call this method with the
  wanted template.

* In general, don't put anything in the traits class that you don't
  expect the user to be able to understand or to change. Don't put
  anything that you don't expect to document. Users care about the
  algorithm being flexible and able to adapt to their data (see the
  input ranges + property mapsn for example), but for example `typedef
  std::vector<Point_with_label> Container_3D` is not interesting for
  users as it's part of an internal algorithm on which they have no
  knowledge and no control.

## General remarks

* There are many things that you did to simplify your life as a
  developer: this is a good practise, but what you need to do now is
  to clearly separate them from the code. You can of course keep them,
  but always _outside_ the package (for example, as additional files
  to your test). You already pretty much started this with your
  `Wrapper` class: I would use this class to put everything that you
  developed to make your life easier (loading files, interpreting
  parameters of the command line, etc.), and leave it unincluded by
  default and undocumented (or even move it to the `test` files).

* Clearly separate in your head three parts:

  - __the public API:__ user-oriented, take documented and general-enough
    data structures as input (input ranges + property maps is
    reasonable, "_the class needs to have the same methods as
    `CGAL::Point_set_3`_" is not), returns standard data structure as
    output (see concept `FaceGraph` for example). 
  
  - __the internal code:__ developer-oriented, you can use your own
    hard-coded data structure, you don't need to document (and you
    shouldn't), you can change it without users noticing.
    
  - __the test programs:__ this is were you can be extra-specific
    about how you handle parameters, how you read the input, what you
    want to use as a data structure (for example,
    `CGAL::Point_set_3`), etc. Tests are not documented and seen by
    the user either (examples are, but that will probably be my part
    of the work).

  Currently, the main problem I see is that these are mixed-up in the
  package. Separating the public API from the internal code is
  something we can work on as the package evolves, but separating the
  testing parts is crucial so we can work on a good basis.
