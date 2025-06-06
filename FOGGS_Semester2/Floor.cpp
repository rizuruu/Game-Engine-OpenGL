#include "Floor.h"

Floor::Floor() : startX(-18), startZ(-18), endx(18), endz(18),
color1{ 0.1f, 0.1f, 0.1f, 1.0f },
color2{ 0.7f, 0.7f, 0.7f, 1.0f } {};
const float scale = 15.0f;
void Floor::draw()
{
    glPushMatrix();
    glScalef(scale, 1.0f, scale); // Apply scaling transformation
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f },
        shininess = 128.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    for (int x = startX; x < endx; x++) {
        for (int z = startZ; z < endz; z++) {
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (x + z) % 2 == 0 ? color1 : color2);
            glVertex3d(x * 0.2, 0, z * 0.2);
            glVertex3d((x + 1) * 0.2, 0, z * 0.2);
            glVertex3d((x + 1) * 0.2, 0, (z + 1) * 0.2);
            glVertex3d(x * 0.2, 0, (z + 1) * 0.2);
        }
    }

    glEnd();
    glPopMatrix();
}
