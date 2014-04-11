#include "irrlicht.h"

#include "Camera.hpp"

using namespace irr;

Camera::Camera()
{

}

Camera::~Camera()
{
    //dtor
}


void Camera::loadCamera(irr::scene::ISceneManager* smgr, irr::scene::IMeshSceneNode* par, irr::core::vector3df off)
{
    camera = smgr->addCameraSceneNode(0, core::vector3df(0,0,0), core::vector3df(0,0,1));
    parent = par;
    offset = off;
}

void Camera::updateCamera()
{
     //link camera rotation to shipNode
        // get transformation matrix of node
        core::matrix4 m;
        m.setRotationDegrees(parent->getRotation());

        // transform forward vector of camera
        core::vector3df frv(0.0f, 0.0f, 1.0f);
        m.transformVect(frv);

        // transform upvector of camera
        core::vector3df upv(0.0f, 1.0f, 0.0f);
        m.transformVect(upv);

        // transform camera offset ('offset' is relative to the local ship coordinates, and stays the same.)
        //'offsetTransformed' is transformed into the global coordinates
        core::vector3df offsetTransformed;
        m.transformVect(offsetTransformed,offset);

        //move camera and angle
        camera->setPosition(parent->getPosition() + offsetTransformed);
        camera->setUpVector(upv); //set up vector of camera
        camera->setTarget(parent->getPosition() + offsetTransformed + frv); //set target of camera (look at point)
        camera->updateAbsolutePosition();

}

void Camera::setPosition(irr::core::vector3df position)
{

}

void Camera::setRotation(irr::core::vector3df rotation)
{

}
