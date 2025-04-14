#pragma once


glm::vec3 DoNothing(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    return vec3(0);
}

glm::vec3 Shade(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    glm::vec3 col(0);
    float small_ambient = 0.1f;
    glm::vec3 surface_col = tri->v1.col;

    col = small_ambient * surface_col;

    //glm::vec3 norm = glm::normalize(glm::cross(tri->v1.pos - tri->v2.pos, tri->v1.pos - tri->v3.pos));
    glm::vec3 norm = glm::normalize(tri->v1.nor);

    glm::vec3 light_dir = glm::normalize(light_pos - p);

    float t_shadow = FLT_MAX;
    glm::vec3 shadow_col(0);

    glm::vec3 shadow_origin = p + 0.0001f * light_dir;
  
    if(depth < max_recursion_depth)
        trace(shadow_origin, light_dir, t_shadow, shadow_col, depth + 1, Shade);

    if (t_shadow == FLT_MAX) {
        float idiff = glm::dot(norm, light_dir);
        if (idiff < 0.0f)
            idiff = 0.0f;
        col += idiff * surface_col;
    }


    if (depth < max_recursion_depth && tri->reflect) {
        glm::vec3 reflection_dir = glm::reflect(dir, norm);
        glm::vec3 reflection_origin = p + 0.0001f * reflection_dir;
        glm::vec3 reflect_col(0);
        float t_reflect = FLT_MAX;

        trace(reflection_origin, reflection_dir, t_reflect, reflect_col, depth + 1, Shade);
        
        float reflecivity = 0.2f;
        col += reflecivity * reflect_col;
    }

    return col;
}

float line(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return ((y2 - y1) * x3) + ((x1 - x2) * y3) + (x2 * y1) - (x1 * y2);
}

bool PointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    glm::vec3 v0 = v3 - v1;
    glm::vec3 v1_ = v2 - v1;
    glm::vec3 v2_ = pt - v1;

    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1_);
    float dot02 = dot(v0, v2_);
    float dot11 = dot(v1_, v1_);
    float dot12 = dot(v1_, v2_);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

float RayTriangleIntersection(glm::vec3 o, glm::vec3 dir, triangle* tri, glm::vec3& point)
{
    glm::vec3 norm = glm::normalize(glm::cross(tri->v2.pos - tri->v1.pos, tri->v3.pos - tri->v1.pos));

    float t = glm::dot(norm, (tri->v1.pos - o)) / glm::dot(norm, dir);
    if (t < 0)
        return FLT_MAX;

    point = o + (dir * t);
    if (PointInTriangle(point, tri->v1.pos, tri->v2.pos, tri->v3.pos))
        return t;
    return FLT_MAX;
}

void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, closest_hit p_hit)
{
    float min = FLT_MAX;
    glm::vec3 pt(0);
    glm::vec3 minpt(0);
    triangle* closest = nullptr;

    for (auto& tri : tris) {
        t = RayTriangleIntersection(o, dir, &tri, pt);
        if (t < min) {
            min = t;
            closest = &tri;
            minpt = pt;
        }
    }

    if (closest != nullptr && p_hit) {
        io_col = p_hit(closest, depth, minpt, dir);
        t = min;
    }
    else {
        io_col = bkgd;
    }
}

vec3 GetRayDirection(float px, float py, int W, int H, float aspect_ratio, float fov)
{
    // Calculate aspect ratio and scale factor
    float a = static_cast<float>(W) / static_cast<float>(H);
    float f = tan(glm::radians(fov) * 0.5f);

    // Define camera basis vectors (assuming a right-handed coordinate system)
    glm::vec3 R(1.0f, 0.0f, 0.0f); // Right
    glm::vec3 U(0.0f, -1.0f, 0.0f); // Up
    glm::vec3 F(0.0f, 0.0f, -1.0f); // Forward

    // Convert pixel coordinate (x,y) into normalized device coordinates (NDC)
    // where x and y range from 0 to 1. Using the pixel center offset of 0.5.
    float ndcX = (px + 0.5f) / W;
    float ndcY = (py + 0.5f) / H;

    // Map NDC to screen space coordinates in [-1,1].
    float screenX = 2.0f * ndcX - 1.0f;
    float screenY = 2.0f * ndcY - 1.0f;

    // Construct the ray direction using the provided formula.
    glm::vec3 d = a * f * screenX * R + f * screenY * U + F;

    return glm::normalize(d);
}

void raytrace()
{
    glm::vec3 col(0.f, 0.f, 0.f);
    float t;
 
    for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
    {
        float percf = (float)pixel_y / (float)PIXEL_H;
        int perci = percf * 100;
        std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

        for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
        {
            glm::vec3 ray = GetRayDirection(pixel_x, pixel_y, PIXEL_W, PIXEL_H,
                static_cast<float>(PIXEL_W) / static_cast<float>(PIXEL_H), 90.f);
            trace(eye, ray, t, col, 0, Shade);
            writeCol(col, pixel_x, pixel_y);
        }
    }
    std::clog << "\rFinish rendering.           \n";
}
