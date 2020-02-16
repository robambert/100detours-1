#include "../include/Point.h"
#include <iostream>
#include <string>
#include <sstream>
#include "../include/Utils.h"
#include <cpprest/json.h>

Point Point::ORIGIN = Point();

Point::Point()
    :m_pos_x(0), m_pos_y(0)
{}

Point::Point(int pos_x, int pos_y)
    :m_pos_x(pos_x), m_pos_y(pos_y)
{}

Point::Point(web::json::value const& jvalue)
	:m_pos_x(jvalue.at(L"x").as_integer()), m_pos_y(jvalue.at(L"y").as_integer())
{}

std::string Point::to_string() const
{
    return "(" + int_to_string(m_pos_x) + ", " + int_to_string(m_pos_y) + ")";
}

bool Point::is_valid_json(web::json::value const& jvalue)
{
	bool has_fields = jvalue.has_integer_field(L"x")
				   && jvalue.has_integer_field(L"y");
	if (!has_fields)
	{
		return false;
	}
	return true;
}

bool operator==(Point const& point1, Point const& point2)
{
    return (point1.pos_x() == point2.pos_x())
        && (point1.pos_y() == point2.pos_y());
}

std::ostream& operator<<(std::ostream &out, Point const& point)
{
    out << point.to_string();
    return out;
}

int distance(Point const& point1, Point const& point2)
{
    return abs(point1.pos_x() - point2.pos_x()) + abs(point1.pos_y() - point2.pos_y());
}
