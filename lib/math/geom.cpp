#include 'math.cpp';
#include 'Line.cpp';

void calculate_rotated_rectangle(
	float x, float y,
	float size_x, float size_y,
	float rotation,
	float &out p1_x, float &out p1_y, float &out p2_x, float &out p2_y,
	float &out p3_x, float &out p3_y, float &out p4_x, float &out p4_y)
{
	float angle = rotation * DEG2RAD;
	float c = cos(angle);
	float s = sin(angle);
	float rx1 = -size_x * c + size_y * s;
	float ry1 = -size_x * s - size_y * c;
	float rx2 =  size_x * c + size_y * s;
	float ry2 =  size_x * s - size_y * c;

	p1_x = x + rx1;
	p1_y = y + ry1;
	p2_x = x + rx2;
	p2_y = y + ry2;
	p3_x = x - rx1;
	p3_y = y - ry1;
	p4_x = x - rx2;
	p4_y = y - ry2;
}

/// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
///  1. The polygon may be concave. However, if a vertex is very close to an edge (that the vertex is not an end of) then beware of roundoff errors.
///  2. The direction that you list the vertices (clockwise or counterclockwise) does not matter.
bool point_in_polygon(const float x, const float y, const array<float>@ points, const int start=0, int end=-1)
{
	bool c = false;
	
	if(end < 0)
	{
		end = int(points.length());
	}
	
	for(int i = start, j = end - 2; i < end; j = i, i += 2)
	{
		if(
			((points[i + 1] > y) != (points[j + 1] > y)) &&
			(x < (points[j] - points[i]) * (y - points[i + 1]) / (points[j + 1] - points[i + 1]) + points[i])
		)
		{
			c = !c;
		}
	}
	
	return c;
}

/// https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
///  1. The polygon may be concave. However, if a vertex is very close to an edge (that the vertex is not an end of) then beware of roundoff errors.
///  2. The direction that you list the vertices (clockwise or counterclockwise) does not matter.
///  3. The polygon may contain multiple separate components, and/or holes, which may be concave.
bool point_in_polygon(const float x, const float y, const array<array<float>>@ paths)
{
	bool c = false;
	
	for(int a = int(paths.length()) - 1; a >= 0; a--)
	{
		const array<float>@ points = @paths[a];
		const int count = int(points.length());
		
		for(int i = 0, j = count - 2; i < count; j = i, i += 2)
		{
			if(
				((points[i + 1] > y) != (points[j + 1] > y)) &&
				(x < (points[j] - points[i]) * (y - points[i + 1]) / (points[j + 1] - points[i + 1]) + points[i])
			)
			{
				c = !c;
			}
		}
	}
	
	return c;
}

float distance_to_polygon_sqr(const float x, const float y, const array<array<float>>@ paths)
{
	float min_distance = MAX_FLOAT;
	Line segment();
	
	for(int a = int(paths.length()) - 1; a >= 0; a--)
	{
		const array<float>@ points = @paths[a];
		const int count = int(points.length());
		
		for(int i = 0, j = count - 2; i < count; j = i, i += 2)
		{
			segment.x1 =  points[i];
			segment.y1 =  points[i + 1];
			segment.x2 =  points[j];
			segment.y2 =  points[j + 1];
			const float distance = segment.distance_squared(x, y);
			
			if(distance < min_distance)
			{
				min_distance = distance;
			}
		}
	}
	
	return min_distance;
}

float distance_to_polygon_sqr(const float x, const float y, const array<float>@ points)
{
	const int count = int(points.length());
	
	float min_distance = MAX_FLOAT;
	Line segment();
	
	for(int i = 0, j = count - 2; i < count; j = i, i += 2)
	{
		segment.x1 =  points[i];
		segment.y1 =  points[i + 1];
		segment.x2 =  points[j];
		segment.y2 =  points[j + 1];
		const float distance = segment.distance_squared(x, y);
		
		if(distance < min_distance)
		{
			min_distance = distance;
		}
	}
	
	return min_distance;
}

void closest_point_to_rect(
	const float x, const float y, const float x1, const float y1, const float x2, const float y2,
	float &out out_x, float &out out_y)
{
	// Top
	
	Line side(x1,  y1, x2, y1);
	side.closest_point(x, y, out_x, out_y);
	float dist = length_sqr(x - out_x, y - out_y);
	
	// Right
	
	side.x1 = x2;
	side.y1 = y1;
	side.x2 = x2;
	side.y2 = y2;
	float new_x, new_y;
	side.closest_point(x, y, new_x, new_y);
	float new_dist = length_sqr(x - new_x, y - new_y);
	
	if(new_dist < dist)
	{
		dist = new_dist;
		out_x = new_x;
		out_y = new_y;
	}
	
	// Bottom
	
	side.x1 = x2;
	side.y1 = y2;
	side.x2 = x1;
	side.y2 = y2;
	side.closest_point(x, y, new_x, new_y);
	new_dist = length_sqr(x - new_x, y - new_y);
	
	if(new_dist < dist)
	{
		dist = new_dist;
		out_x = new_x;
		out_y = new_y;
	}
	
	// Left
	
	side.x1 = x1;
	side.y1 = y2;
	side.x2 = x1;
	side.y2 = y1;
	side.closest_point(x, y, new_x, new_y);
	new_dist = length_sqr(x - new_x, y - new_y);
	
	if(new_dist < dist)
	{
		dist = new_dist;
		out_x = new_x;
		out_y = new_y;
	}
}

bool point_in_triangle(
	const float x, const float y, const float x1, const float y1, const float x2, const float y2,
	const float x3, const float y3)
{
	// Not fully tested but seems to also returns true for points on a triangle edge
	const float l1 = (x - x1) * (y3 - y1) - (x3 - x1) * (y - y1);
	const float l2 = (x - x2) * (y1 - y2) - (x1 - x2) * (y - y2);
	const float l3 = (x - x3) * (y2 - y3) - (x2 - x3) * (y - y3);
	return
		(l1 >= 0 && l2 >= 0 && l3 >= 0) ||
		(l1 <= 0 && l2 <= 0 && l3 <= 0);
	
	// const float s = y1 * x3 - x1 * y3 + (y3 - y1) * x + (x1 - x3) * y;
	// const float t = x1 * y2 - y1 * x2 + (y1 - y2) * x + (x2 - x1) * y;
	// 
	// if((s <= 0) != (t <= 0))
	// 	return false;
	// 
	// const float a = -y2 * x3 + y1 * (x3 - x2) + x1 * (y2 - y3) + x2 * y3;
	// 
	// return a < 0
	// 	? (s <= 0 && s + t >= a)
	// 	: (s >= 0 && s + t <= a);
}

/// https://stackoverflow.com/a/402010/153844
bool circle_rectangle_intersect(
	const float cx, const float cy, const float radius,
	const float x, const float y, const float hw, const float hh)
{
	const float dx = abs(cx - x);
	const float dy = abs(cy - y);

	if(dx > hw + radius)
		return false;
	if(dy > hh + radius)
		return false;

	if(dx <= hw)
		return true;
	if(dy <= hh)
		return true;

	const float corner_dist_sqr = (dx - hw) * (dx - hw) + (dy - hh) * (dy - hh);

	return corner_dist_sqr <= radius * radius;
}

/// https://stackoverflow.com/a/10392860/153844
bool line_circle_intersect(
	const float cx, const float cy, const float radius,
	const float x1, const float y1, const float x2, const float y2)
{
	const float acx = cx - x1;
	const float acy = cy - y1;
	const float abx = x2 - x1;
	const float aby = y2 - y1;
	const float ab2 = abx * abx + aby * aby; // dot(ab, ab)
	const float acab = acx * abx + acy * aby; // dot(ac, ab)
	float t = acab / ab2;
	
	if(t < 0)
	{
		t = 0;
	}
	else if(t > 1)
	{
		t = 1;
	}
	
	const float hx = abx * t + x1 - cx;
	const float hy = aby * t + y1 - cy;
	const float h2 = hx * hx + hy * hy; // dot(h, h)

	return h2 <= radius * radius;
}
