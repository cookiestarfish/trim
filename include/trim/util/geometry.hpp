#pragma once
#include <cmath>
#include <compare>
#include <cstdint>
#include <format>

#include <trim/util/axis.hpp>
#include <trim/util/direction.hpp>
#include <trim/util/ints.hpp>

namespace trim
{
  struct Point
  {
    static constinit Point const origin;

    coord_type line {};
    coord_type column {};

    Point() = default;

    constexpr Point(coord_type line, coord_type column) noexcept
      : line(line)
      , column(column)
    {}

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Point const& p)
    {
      return stream << "point(" << p.line << ", " << p.column << ")";
    }

    bool operator==(Point const&) const = default;
    auto operator<=>(Point const&) const = default;
  };

  constexpr Point Point::origin = Point(0, 0);

  struct Vertical_Segment
  {
    Point p1 {};
    Point p2 {};

    Vertical_Segment() = default;

    constexpr Vertical_Segment(Point p1, Point p2) noexcept
      : p1(p1)
      , p2(p2)
    {
      TRIM_ASSERT(p1.column == p2.column);
    }

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Vertical_Segment const& segment)
    {
      return stream << "segment_v(" << segment.p1 << ", " << segment.p2 << ")";
    }

    bool operator==(Vertical_Segment const&) const = default;
    auto operator<=>(Vertical_Segment const&) const = default;
  };

  struct Horizontal_Segment
  {
    Point p1 {};
    Point p2 {};

    Horizontal_Segment() = default;

    constexpr Horizontal_Segment(Point p1, Point p2) noexcept
      : p1(p1)
      , p2(p2)
    {
      TRIM_ASSERT(p1.line == p2.line);
    }

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Horizontal_Segment const& segment)
    {
      return stream << "segment_h(" << segment.p1 << ", " << segment.p2 << ")";
    }

    bool operator==(Horizontal_Segment const&) const = default;
    auto operator<=>(Horizontal_Segment const&) const = default;
  };

  struct Rect
  {
    Point p1 {};
    Point p2 {};

    Rect() = default;

    constexpr Rect(Point p1, Point p2) noexcept
      : p1(p1)
      , p2(p2)
    {}

    template<typename Stream>
    friend constexpr Stream& operator<<(Stream& stream, Rect r)
    {
      return stream << "rect(" << r.p1 << ", " << r.p2 << ")";
    }

    bool operator==(Rect const&) const = default;
  };

  [[nodiscard]] constexpr Point firstpoint(Vertical_Segment const& segment) noexcept
  {
    return segment.p1;
  }

  [[nodiscard]] constexpr Point firstpoint(Horizontal_Segment const& segment) noexcept
  {
    return segment.p1;
  }

  [[nodiscard]] constexpr Point lastpoint(Vertical_Segment const& segment) noexcept
  {
    return segment.p2;
  }

  [[nodiscard]] constexpr Point lastpoint(Horizontal_Segment const& segment) noexcept
  {
    return segment.p2;
  }

  [[nodiscard]] constexpr Point midpoint(Point const& p1, Point const& p2) noexcept
  {
    coord_type line = std::midpoint(p1.line, p2.line);
    coord_type column = std::midpoint(p1.column, p2.column);
    return Point(line, column);
  }

  [[nodiscard]] constexpr Point midpoint(Vertical_Segment const& segment) noexcept
  {
    return trim::midpoint(segment.p1, segment.p2);
  }

  [[nodiscard]] constexpr Point midpoint(Horizontal_Segment const& segment) noexcept
  {
    return trim::midpoint(segment.p1, segment.p2);
  }

  [[nodiscard]] constexpr Axis axis(Vertical_Segment const&) noexcept
  {
    return Axis::VERTICAL;
  }

  [[nodiscard]] constexpr Axis axis(Horizontal_Segment const&) noexcept
  {
    return Axis::HORIZONTAL;
  }

  [[nodiscard]] constexpr Axis axis(Point const& p1, Point const& p2) noexcept
  {
    return trim::axis(p1.line, p1.column, p2.line, p2.column);
  }

  [[nodiscard]] constexpr Direction direction(Vertical_Segment const& segment) noexcept
  {
    return segment.p1.line < segment.p2.line ? Direction::DOWN : Direction::UP;
  }

  [[nodiscard]] constexpr Direction direction(Horizontal_Segment const& segment) noexcept
  {
    return segment.p1.column < segment.p2.column ? Direction::RIGHT : Direction::LEFT;
  }

  [[nodiscard]] constexpr Direction direction(Point const& p1, Point const& p2) noexcept
  {
    return trim::direction(p1.line, p1.column, p2.line, p2.column);
  }

  [[nodiscard]] constexpr coord_type signed_length(Horizontal_Segment const& segment) noexcept
  {
    return segment.p2.column - segment.p1.column;
  }

  [[nodiscard]] constexpr coord_type length(Horizontal_Segment const& segment) noexcept
  {
    coord_type result = trim::signed_length(segment);
    return result < 0 ? -result : result;
  }

  [[nodiscard]] constexpr coord_type signed_length(Vertical_Segment const& segment) noexcept
  {
    return segment.p2.line - segment.p1.line;
  }

  [[nodiscard]] constexpr coord_type length(Vertical_Segment const& segment) noexcept
  {
    coord_type result = trim::signed_length(segment);
    return result < 0 ? -result : result;
  }

  [[nodiscard]] constexpr coord_type signed_distance_vertical(Point const& p1, Point const& p2) noexcept
  {
    return p2.line - p1.line;
  }

  [[nodiscard]] constexpr coord_type signed_distance_horizontal(Point const& p1, Point const& p2) noexcept
  {
    return p2.column - p1.column;
  }

  [[nodiscard]] constexpr Point translate(Point const& p, coord_type lines, coord_type columns) noexcept
  {
    return Point(p.line + lines, p.column + columns);
  }

  [[nodiscard]] constexpr Point translate(Point const& p, Direction dir, coord_type amount = +1) noexcept
  {
    return trim::translate(p, trim::line_magnitude(dir) * amount, trim::column_magnitude(dir) * amount);
  }

  [[nodiscard]] constexpr Point translate(Point const& p1, Point const& p2, coord_type amount = +1) noexcept
  {
    return trim::translate(p1, trim::direction(p1, p2), amount);
  }

  [[nodiscard]] constexpr Rect translate(Rect const& rect, coord_type lines, coord_type columns) noexcept
  {
    Point p1 = trim::translate(rect.p1, lines, columns);
    Point p2 = trim::translate(rect.p2, lines, columns);
    return Rect(p1, p2);
  }

  [[nodiscard]] constexpr Rect translate(Rect const& rect, Direction dir, coord_type amount = +1) noexcept
  {
    return trim::translate(rect, trim::line_magnitude(dir) * amount, trim::column_magnitude(dir) * amount);
  }

  [[nodiscard]] constexpr coord_type top_line(Rect const& rect) noexcept
  {
    return std::min(rect.p1.line, rect.p2.line);
  }

  [[nodiscard]] constexpr coord_type bot_line(Rect const& rect) noexcept
  {
    return std::max(rect.p1.line, rect.p2.line);
  }

  [[nodiscard]] constexpr coord_type left_column(Rect const& rect) noexcept
  {
    return std::min(rect.p1.column, rect.p2.column);
  }

  [[nodiscard]] constexpr coord_type right_column(Rect const& rect) noexcept
  {
    return std::max(rect.p1.column, rect.p2.column);
  }

  [[nodiscard]] constexpr Point top_left_corner(Rect const& rect) noexcept
  {
    return Point(trim::top_line(rect), trim::left_column(rect));
  }

  [[nodiscard]] constexpr Point top_right_corner(Rect const& rect) noexcept
  {
    return Point(trim::top_line(rect), trim::right_column(rect));
  }

  [[nodiscard]] constexpr Point bot_left_corner(Rect const& rect) noexcept
  {
    return Point(trim::bot_line(rect), trim::left_column(rect));
  }

  [[nodiscard]] constexpr Point bot_right_corner(Rect const& rect) noexcept
  {
    return Point(trim::bot_line(rect), trim::right_column(rect));
  }

  [[nodiscard]] constexpr Horizontal_Segment top_segment(Rect const& rect) noexcept
  {
    return Horizontal_Segment(trim::top_left_corner(rect), trim::top_right_corner(rect));
  }

  [[nodiscard]] constexpr Horizontal_Segment bot_segment(Rect const& rect) noexcept
  {
    return Horizontal_Segment(trim::bot_left_corner(rect), trim::bot_right_corner(rect));
  }

  [[nodiscard]] constexpr Vertical_Segment right_segment(Rect const& rect) noexcept
  {
    return Vertical_Segment(trim::top_right_corner(rect), trim::bot_right_corner(rect));
  }

  [[nodiscard]] constexpr Vertical_Segment left_segment(Rect const& rect) noexcept
  {
    return Vertical_Segment(trim::top_left_corner(rect), trim::bot_left_corner(rect));
  }

  [[nodiscard]] constexpr coord_type height(Rect const& rect) noexcept
  {
    return rect.p2.line - rect.p1.line;
  }

  [[nodiscard]] constexpr coord_type width(Rect const& rect) noexcept
  {
    return rect.p2.column - rect.p1.column;
  }

  [[nodiscard]] constexpr bool envelopes(Rect const& rect, Point const& point) noexcept
  {
    bool inside_v = point.line >= trim::top_line(rect) && point.line <= trim::bot_line(rect);
    bool inside_h = point.column >= trim::left_column(rect) && point.column <= trim::right_column(rect);
    return inside_v && inside_h;
  }

  [[nodiscard]] constexpr bool envelopes(Horizontal_Segment const& segment, Point const& point) noexcept
  {
    coord_type c1 = std::min(segment.p1.column, segment.p2.column);
    coord_type c2 = std::max(segment.p1.column, segment.p2.column);
    return point.line == segment.p1.line && (point.column >= c1 && point.column <= c2);
  }

  [[nodiscard]] constexpr bool envelopes(Vertical_Segment const& segment, Point const& point) noexcept
  {
    coord_type l1 = std::min(segment.p1.line, segment.p2.line);
    coord_type l2 = std::max(segment.p1.line, segment.p2.line);
    return point.column == segment.p1.column && (point.line >= l1 && point.line <= l2);
  }

  [[nodiscard]] constexpr Rect minumum_bounding_box(Rect const& lhs, Rect const& rhs) noexcept
  {
    coord_type min_line = std::min(trim::top_line(lhs), trim::top_line(rhs));
    coord_type max_line = std::max(trim::bot_line(lhs), trim::bot_line(rhs));
    coord_type min_column = std::min(trim::left_column(lhs), trim::left_column(rhs));
    coord_type max_column = std::max(trim::right_column(lhs), trim::right_column(rhs));
    return Rect(Point(min_line, min_column), Point(max_line, max_column));
  }

  [[nodiscard]] constexpr bool on_horizontal_segments(Rect const& rect, Point const& point) noexcept
  {
    return trim::envelopes(trim::top_segment(rect), point) || trim::envelopes(trim::bot_segment(rect), point);
  }

  [[nodiscard]] constexpr bool on_vertical_segments(Rect const& rect, Point const& point) noexcept
  {
    return trim::envelopes(trim::left_segment(rect), point) || trim::envelopes(trim::right_segment(rect), point);
  }

  [[nodiscard]] constexpr bool on_corner(Rect const& rect, Point const& point) noexcept
  {
    return trim::on_vertical_segments(rect, point) && trim::on_horizontal_segments(rect, point);
  }

} // namespace trim
