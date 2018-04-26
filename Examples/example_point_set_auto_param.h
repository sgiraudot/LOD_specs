#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>

#include <CGAL/Level_of_detail/Level_of_detail.h>
#include <CGAL/Surface_mesh.h>

#include <vector>
#include <fstream>

// Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef CGAL::Point_set_3<Point> Point_set;

// Output
typedef CGAL::Surface_mesh<Point> Surface_mesh;

namespace LOD = CGAL::Level_of_detail;

typedef LOD::Level_of_detail_traits<Kernel, Point_set, Point_set::Point_map, Point_set::Vector_map> LOD_traits;
typedef LOD::Level_of_detail<LOD_traits> Level_of_detail;

// LOD needs to know if a point is outside, inside or boundary. In
// this particular example, we compute it on the fly with the normal
// value and the height value (very naive implementation just for the
// sake of the example).
struct Naive_point_type_map
{
  typedef Point_set::Index key_type;
  typedef Level_of_detail::Point_type value_type;
  typedef value_type reference;
  typedef boost::readable_property_map_tag category;

  Point_set* points;
  double ground_level;

  Naive_point_type_map (Point_set& points, double ground_level)
    : points (&points), ground_level (ground_level)
  {
  }

  inline friend reference
  get (const Naive_point_type_map& pmap, key_type index)
  {
    const Vector& n = pmap.points->normal(index);
    if (CGAL::abs(n * Vector(0., 0., 1.)) < 0.1) // if normal is horizontal, it's a wall
      return Level_of_detail::BOUNDARY;

    // Else, we check the height to see it if's ground or roof
    const Point& p = pmap.points->point(index);
    
    if (p.z() > ground_level) // It's a roof
      return Level_of_detail::INSIDE;

    // Else it's ground
    return Level_of_detail::OUTSIDE;
  }

};

int main (int argc, char **argv)
{
  const char* fname = argv[1];
  
  std::ifstream in(fname);
  Point_set points;
  in >> points;

  double spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>
    (points, 6, CGAL::parameters::point_map(Point_map()));
  
  Level_of_detail lod (points, points.point_map(), points.normal_map());

  Naive_point_type_map point_type_map (points,
                                       2.0); // let's say all roof are at least 2m high
  
  Level_of_detail::LOD0_parameters params (spacing, // epsilon
                                           3. * spacing, // cluster epsilon
                                           0.7); // normal tolerance
  
  lod.build_lod0 (point_type_map, params);
  Surface_mesh lod0;
  lod.output_lod0_to_face_graph (lod0);

  lod.build_lod1 ();
  Surface_mesh lod1;
  lod.output_lod1_to_face_graph (lod1);
  
  return EXIT_SUCCESS;
}

