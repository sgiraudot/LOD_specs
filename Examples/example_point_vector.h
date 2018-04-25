#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/read_ply_points.h>
#include <CGAL/Level_of_detail/Level_of_detail.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>

#include <vector>
#include <fstream>
#include <boost/tuple/tuple.hpp>

// Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

// Input points
typedef CGAL::cpp11::tuple<Point, Vector, int> Point_with_info;
typedef CGAL::Nth_of_tuple_property_map<0, Point_with_info> Point_map;
typedef CGAL::Nth_of_tuple_property_map<1, Point_with_info> Normal_map;
typedef CGAL::Nth_of_tuple_property_map<2, Point_with_info> Label_map;
typedef std::vector<Point_with_info> Point_range;

// Output
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Surface_mesh<Point> Surface_mesh;

namespace LOD = CGAL::Level_of_detail;

typedef LOD::Level_of_detail_traits<Kernel, Point_range, Point_map, Normal_map> LOD_traits;
typedef LOD::Level_of_detail<LOD_traits> Level_of_detail;


// LOD needs to know if a point is outside, inside or boundary. In
// this particular example, we have classification indices. This
// property map takes a Point_with_info and returns the correct LOD::Point_type.
struct Label_to_point_type_map
{
  typedef Point_with_info key_type;
  typedef Level_of_detail::Point_type value_type;
  typedef value_type reference;
  typedef boost::readable_property_map_tag category;

  inline friend reference
  get (const Label_to_point_type_map&, key_type point_with_info)
  {
    int label = get<2>(point_with_info);
    if (label == 0) // Let's say that 0 means ground
      return Level_of_detail::OUTSIDE;
    if (label == 1) // Let's say that 1 means roof
      return Level_of_detail::INSIDE;
    if (label == 2) // Let's say that 2 means wall
      return Level_of_detail::BOUNDARY;

    # else
    return Level_of_detail::UNKNOWN;
  }

};

int main (int argc, char **argv)
{
  const char* fname = argv[1];
  
  std::ifstream in(fname);
  Point_range points;

  if (!in ||
      !CGAL::read_ply_points_with_properties
      (in,
       std::back_inserter (points),
       CGAL::make_ply_point_reader (Point_map()),
       CGAL::make_ply_normal_reader (Normal_map()),
       std::make_pair (Label_map(),
                       CGAL::PLY_property<int>("label"))
        ))
  {
    std::cerr << "Error: cannot read file " << fname << std::endl;
    return EXIT_FAILURE;
  }

  double spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>
    (points, 6, CGAL::parameters::point_map(Point_map()));
  
  Level_of_detail lod (points);

  lod.build_lod0 (Label_to_point_type_map(), spacing, 3. * spacing);
  Polyhedron lod0;
  lod.output_lod0_to_face_graph (lod0);

  lod.build_lod1 ();
  Surface_mesh lod1;
  lod.output_lod1_to_face_graph (lod1);
  
  return EXIT_SUCCESS;
}

