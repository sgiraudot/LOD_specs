#ifndef CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H
#define CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H

namespace CGAL {

namespace LOD {

template <typename LodTraits>
class Level_of_detail
{
public:

  enum Point_type
  {
    UNKNOWN = -1,
    INSIDE,
    OUTSIDE,
    BOUNDARY
  };

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

  Level_of_detail (const PointRange& points,            /// Input point range
                   PointMap point_map = PointMap(),     /// Property map to access points
                   NormalMap normal_map = NormalMap()); /// Property map to access normals

  /// PointTypeMap takes a `PointRange::const_iterator::value_type` as input and returns a `Point_type`
  template <typename PointTypeMap>
  void build_lod0 (PointTypeMap point_type_map,            /// Property map to access the type of point (INSIDE, OUTSIDE, etc.)
                   double epsilon,                         /// Noise tolerance
                   double cluster_epsilon,                 /// Sampling tolerance
                   double normal_threshold = 0.9,          /// Normal variation tolerance
                   std::size_t min_number_of_points = 10); /// Minimum number of points per wall

  /// By default, we use the average height of roofs
  void build_lod1 (const Lod1_method& method = AVERAGE);
  

  /// Output LOD0 to a mesh (FaceGraph concept has, for example, `CGAL::Polyhedron_3` or `CGAL::Surface_mesh` as models) 
  template <typename FaceGraph>
  void output_lod0_to_face_graph (FaceGraph& mesh);

  /// Output LOD1 to a mesh (FaceGraph concept has, for example, `CGAL::Polyhedron_3` or `CGAL::Surface_mesh` as models) 
  template <typename FaceGraph>
  void output_lod1_to_face_graph (FaceGraph& mesh);

  
};
  
}

}

#endif // CGAL_LEVEL_OF_DETAIL_LEVEL_OF_DETAIL_H
