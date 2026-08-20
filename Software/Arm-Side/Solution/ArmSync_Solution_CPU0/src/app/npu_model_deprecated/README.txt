This model inputs Elbow(x, y, z) and Wrist(x, y, z) vectors, and outputs J3 and J5 degrees.
Input vectors needs such preprocessing:

// Input pre-processing (many magic numbers...)
// Normalize a 3-component point (in-place) using its own mean / population std.
// scale: per-point divisor applied to the raw mm values first.
void _normalizePoint(float out[3], const float raw[3], bool dotIsElbow) {
    float scale = 500.0f;
    dotIsElbow ? scale = 500.0f : scale = 700.0f;

    const float x = raw[0] / scale;
    const float y = raw[1] / scale;
    const float z = raw[2] / scale;

    const float avg[6]   = {0.041949, 0.118567, -0.529775, 0.188770, 0.038480, -0.493504};
    const float sigma[6] = {0.154188, 0.171552,  0.084818, 0.147490, 0.255945,  0.130837};

    uint8_t i = 0;
    dotIsElbow ? i = 0 : i = 3;

    const float x1 = x - avg[0 + i];
    const float y1 = y - avg[1 + i];
    const float z1 = z - avg[2 + i];

    out[0] = x1 / sigma[0 + i];
    out[1] = y1 / sigma[1 + i];
    out[2] = z1 / sigma[2 + i];
}

This model has been deprecated.