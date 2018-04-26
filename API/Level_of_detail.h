#ifndef CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H
#define CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H

namespace CGAL {

namespace LOD {

template <typename LodTraits>
class Level_of_detail
{
public:

  /// This is a point type *as seen from LOD*: the fact that it comes
  /// from classification, manual selection or something else is
  /// entirely up to the user (see `PointTypeMap` in `build_lod0`)
  enum Point_type
  {
    UNKNOWN = -1,
    INSIDE,
    OUTSIDE,
    BOUNDARY
  };
  // Note: I'm not sure this is enough information. This is just a
  // proposal, but if you need more detailed types, feel free to edit.

  /// Selection of how roofs are estimated for LOD1
  enum Lod1_method
  {
    MINIMUM,
    AVERAGE,
    MEDIAN,
    MAXIMUM
  };

  typedef LodTraits Traits;

  /// Input point range
  typedef typename Traits::Kernel Kernel;
  
  /// Point range type. It's iterator value_type is the key_type of the property maps.
  typedef typename Traits::PointRange PointRange;
  
  /// Map that takes a `PointRange::const_iterator::value_type` as input and returns a `Kernel::Point_3` object
  typedef typename Traits::PointMap PointMap;

  /// Map that takes a `PointRange::const_iterator::value_type` as input and returns a `Kernel::Vector_3` object
  typedef typename Traits::NormalMap NormalMap;

  Level_of_detail (const PointRange& points,            /// Input point range (should be stored as a conf ref)
                   PointMap point_map = PointMap(),     /// Property map to access points (should be stored by value)
                   NormalMap normal_map = NormalMap()); /// Property map to access normals (should be stored by value)

  /// PointTypeMap takes a `PointRange::const_iterator::value_type` as input and returns a `Point_type`
  template <typename PointTypeMap>
  void build_lod0 (PointTypeMap point_type_map,            /// Property map to access the type of point (INSIDE, OUTSIDE, etc.)
                   double epsilon,                         /// Noise tolerance
                   double cluster_epsilon,                 /// Sampling tolerance
                   double normal_threshold = 0.9,          /// Normal variation tolerance
                   std::size_t min_number_of_points = 10, /// Minimum number of points per wall
                   double graphcut_alpha = 1.0,
                   double graphcut_beta = 100000.0,
                   double graphcut_gamma = 10000.0);
  // Note: there might be some more parameters, but most of them
  // should be deduced from these ones if I'm not mistaken (it's
  // unrealistic to expect the user to tune in more than 5 parameters)

  // Note: your idea of a parameter class is okay too, but in that
  // case, use the same principle as in RANSAC/Region_growing: just
  // create it as a subclass of `Level_of_detail` (it doesn't need to
  // be a template as it just uses simple types) that will be passed
  // and that can be automatically computed if default is given.

  // Example of parameter class:
  struct LOD0_parameters
  {
    double epsilon;                         /// Noise tolerance
    double cluster_epsilon;                 /// Sampling tolerance
    double normal_threshold;
    std::size_t min_number_of_points;
    double graphcut_alpha;
    double graphcut_beta;
    double graphcut_gamma;
    
    LOD0_parameters (...); // initialize with user defined values (or not)
  };

  // Alternative implementation of LOD0 with a parameter class
  template <typename PointTypeMap>
  void build_lod0 (PointTypeMap point_type_map,
                   const LOD0_parameters& parameters = LOD0_parameters());


  /// By default, we use the average height of roofs
  void build_lod1 (const Lod1_method& method = AVERAGE);
  

  /// Output LOD0 to a mesh (FaceGraph concept has, for example, `CGAL::Polyhedron_3` or `CGAL::Surface_mesh` as models) 
  template <typename FaceGraph>
  void output_lod0_to_face_graph (FaceGraph& mesh) const;

  /// Output LOD1 to a mesh (FaceGraph concept has, for example, `CGAL::Polyhedron_3` or `CGAL::Surface_mesh` as models) 
  template <typename FaceGraph>
  void output_lod1_to_face_graph (FaceGraph& mesh) const;

  // Note: this would only be the first version of a public API: you
  // can have as many more undocumented methods as you want
  // (especially for testing and developing), but the algorithms
  // should work when we call the previous functions.
  
};
  
}

}

#endif // CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H
