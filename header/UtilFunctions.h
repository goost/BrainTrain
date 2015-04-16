#pragma once

#include <osg/Camera>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>
#include <osgDB/ReadFile>
#include <osg/Shader>
#include <osg/Material>
#include <osgParticle/ParticleSystem>
#define _USE_MATH_DEFINES
#include <cmath> 
#include <functional>

/**
* @file
* @author Gleb Ostrowski & Marcel Felix
*/

/**
 * @brief Namespace for the whole BrainTrain Project.
*/
namespace brtr {
    const int collisionMask = 0x1;      
    const int interactionMask = 0x2;
    const int interactionAndCollisionMask = collisionMask | interactionMask;
    const int fakeWallMask = 0x4;


    /**
    * @brief struct holding the function, which calculates the radius in dependece of the height.
    *           lambda (double)->double func, int end, BodyOfRotationFunction* nextFunc
    *           if one wish to have more then one function then the end value and nextFunc pointer must be set accordingly
    *           the end+1 is the beginning x of the next function
    *           
    */
    struct  BodyOfRotationFunction {
        std::function<double(double)> func;     ///< the function
        double end;                                 ///< the end value of the function, should be less or equal createBodyOfRotation::height
        const BodyOfRotationFunction* nextFunc; ///< if end is less then createBodyOfRotation::height, must point towards the next function which shall be used from end
        double derivation(double x) const {
            double h = 1e-10; //very small
            return (func(x + h) - func(x)) / h;
        };
    };

    /**
    * @brief struct holding the camera for the multi-rendering passes. Also holds the program vector for the post process pass.
    *           pass0Color, pass0depth, passPostProcess, program array, count programArray
    *         The program vector is used by the KeyHandler and the InteractionItems for changing the postprocess programs
    *
    */
    struct RenderingPipeline {
        osg::ref_ptr<osg::Camera> pass_0_color;             ///< Camera for the first pass, renders the ColorBuffer to Texture
        osg::ref_ptr<osg::Camera> pass_0_depth;             ///< Camera for the first pass, renders the DepthBuffer to Texture
        osg::ref_ptr<osg::Camera> pass_PostProcess;         ///< PostProcess Camera, uses the texture from the first pass to create various effects
        std::vector<osg::ref_ptr<osg::Program>> programs;   ///< vector with the avaible postprocess programs
    };

    /**
     * @brief creates the rendering pipeline
     *
     *  Creates the cameras and textures, attachs the textures to the cameras, set the projectionmatrix
     * 
     *
     * @param width         the width of the texture, should be screenwidth 
     * @param height        the height of the texture, should be screenheight
     * @param rootForToon   Node which the CelShade effect will be applied to 
     * @param viewer        clipping pane and projectionmatrix will be set on this viewers cam        
     * @param pipe          pipe struct which should be filled
     */
    extern void createRenderingPipeline(unsigned int width, unsigned int height, osg::Node& rootForToon, osgViewer::Viewer &viewer, RenderingPipeline& pipe, osg::Vec3f& fogColor);
    
    /**
     * @brief creates a Light with a lightsource
     *
     * @param pos       light position 
     * @param lightNum  gl_light num (must be 0 to 7)
     * @param point     1 = point light, 0 = directional light
     * @param spotCutoff 
     * @param spotExponent 
     * @return              a ref_ptr containing the LightSource 
     */
    extern osg::ref_ptr<osg::LightSource> createLight(const osg::Vec3 &pos, int lightNum, int point = 1, double spotCutoff = 180, double spotExponent = 0);
    /**
     * @brief creates a RTTCam
     *
     * Original Function by Rui Wang/Xuelei Qian from OSG 3 Cookbook, Packt Publishing, 2012
     *
     * @param buffer        which buffer should be written to texture 
     * @param tex           on this texture the buffer will be written to
     * @param isAbsolute    absolute or relative reference frame 
     * @return              a ref_ptr holding the camera
     */
    extern osg::ref_ptr<osg::Camera> createRTTCamera(osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute = false);
    /**
     * @brief creates a texture-ready screen quad for postprocessing
     *
     * Original Function by Rui Wang/Xuelei Qian from OSG 3 Cookbook, Packt Publishing, 2012
     *
     * @param width     width of the quad 
     * @param height    height of the quad 
     * @param scale     scale of the quad
     * @return          a ref_ptr caontaining the geode with the quad 
     */
    extern osg::ref_ptr<osg::Geode> createScreenQuad(float width, float height, float scale = 1.0f);
    /**
     * @brief creates a HUD-Cam with a 2D-orthogonal projection matrix
     *
     * Original Function by Rui Wang/Xuelei Qian from OSG 3 Cookbook, Packt Publishing, 2012
     *
     * @param left      left bound of the projection matrix
     * @param right     right bound of the projection matrix
     * @param bottom    bottom bound of the projection matrix
     * @param top       top bound of the projection matrix 
     * @return          the created HUD Camera in a ref_ptr
     */
    extern osg::ref_ptr<osg::Camera> createHUDCamera(double left, double right, double bottom, double top);
    /**
     * @brief creates a (arial) text object for use with a hud camera
     *
     * Original Function by Rui Wang/Xuelei Qian from OSG 3 Cookbook, Packt Publishing, 2012
     
     * @param pos       postion of the text in x_y plane
     * @param content   
     * @param size 
     * @return      a ref_ptr containing the osgText::Text object
     */
    extern osg::ref_ptr<osgText::Text> createText(const osg::Vec3& pos, const std::string& content, float size);
    /**
     * @brief Creates a body of rotation
     *
     * Radius depends on height. (i.e. function x is height)
     * Function is a modified  createRectangle() from Chapter 7, CG1 Lecture Script by Frauke Sprengel
     *
     * @param height the height of the body 
     * @param hsteps height resolution, more steps equals more triangles, hence better lightning, but more performance cost
     * @param rsteps radius resolution, if rsteps value is too small, the cylinder may become a triangle or something else
     * @param function a BodyOfRotationFunction, which determines the radius in dependence of the height 
     * @return a ref_ptr<osg::Geometry> containing the body
     */
    extern osg::ref_ptr<osg::Geometry> createBodyOfRotation(double height, int hsteps, int rsteps, const BodyOfRotationFunction& function);
    /**
    * @brief Creates a Rectangle with TRIANGLE_STRIPS
    *
    * Function is copy/pasted from Chapter 7, CG1 Lecture Script by Frauke Sprengel
    *
    * @param length desired length of the rectangle
    * @param width desired width of the rectangle
    * @param lsteps the higher the number the greater the resolution in the length dimension
    * @param wsteps the higher the number the greater the resolution in the width dimension
    * @return a ref_ptr<osg::Geometry> containing the rectangle
    */
    extern osg::ref_ptr<osg::Geometry> createRectangle(double length, double width, int lsteps, int wsteps);

	/**
	* @brief Creates a Rectangle with TRIANGLE_STRIPS
	*
	* Function is copy/pasted from Chapter 7, CG1 Lecture Script by Frauke Sprengel
	* Added the TexCoordArray. If the width/length ratio lower than 1:4 or 4:1 the texture coordiantes are streched to fit the Rectangle
	*
	* @param length desired length of the rectangle
	* @param width desired width of the rectangle
	* @param lsteps the higher the number the greater the resolution in the length dimension
	* @param wsteps the higher the number the greater the resolution in the width dimension
	* @return a ref_ptr<osg::Geometry> containing the rectangle
	*/
    extern osg::ref_ptr<osg::Geometry> createRectangleWithTexcoords(double length, double width, int lsteps, int wsteps);

    /**
    * @brief Creates a Cubiod with TRIANGLE_STRIPS using the createRectangle function
    *
    * Uses 6 Rectangles and creates a Cuboid of it.
    *
    * @param length desired length of the Cuboid
    * @param width desired length of the Cuboid
    * @param height desired length of the Cuboid
    * @param factor the higher the number the greater the resolution in all dimension
    * @return a osg::Group containing the Cuboid
    */
    extern osg::ref_ptr<osg::Group> createCuboid(const double length, const double width, const double height, const double factor = 6);
    
	/**
	* @brief Return the given Node in a PositionAttitudeTransform with a given position
	*
	* @param srcNode the Node which should be moved
	* @param pos the relative position change
	* @return a osg::PositionAttitudeTransform containing the Cuboid
	*/
	extern osg::ref_ptr<osg::PositionAttitudeTransform> wrapInPositionAttitudeTransform(osg::Node * srcNode, const osg::Vec3d& pos);

    /**
     * @brief Creates a BeerBottle with Material with the help of the BodyOfRotationFunction
     * @return  ref_ptr containing the geometry
     */
    extern osg::ref_ptr<osg::Geometry> createBeerBottle();
    /**
     * @brief Creates a Bottle with Material with the help of the BodyOfRotationFunction
     *
     * Function provided by Florian Wicke
     *
     * @return ref_ptr containing the geometry
     */
    extern osg::ref_ptr<osg::Geometry> createRealBottle();
    /**
     * @brief Creates a vase with Material with the help of the BodyOfRotationFunction
     *
     * Function provided by Florian Wicke
     *
     * @return ref_ptr containing the geometry
     */
    extern osg::ref_ptr<osg::Geometry> createVase();
    /**
     * @brief Creates a stalk with Material with the help of the BodyOfRotationFunction
     *
     * Function provided by Florian Wicke
     *
     * @return ref_ptr containing the geometry
     */
    extern  osg::ref_ptr<osg::Geometry> createStalk();
    /**
     * @brief Creates a bud with Material with the help of the BodyOfRotationFunction
     *
     * Function provided by Florian Wicke
     *
     * @return ref_ptr containing the geometry 
     */
    extern  osg::ref_ptr<osg::Geometry> createBud();
    /**
     * @brief Creates a "ChessFigure" with Material with the help of the BodyOfRotationFunction
     *
     * Function provided by Florian Wicke
     *
     * @return  ref_ptr containing the geometry 
     */
    extern osg::ref_ptr<osg::Geometry> createChessFigure();

    /**
     * @brief combines the stalk, bud and vase in a postitionAttitudetransform
     *
     * @return a ref_ptr containing a positionAttitudeTransform containing the vase with a flower 
     */
    extern osg::ref_ptr<osg::PositionAttitudeTransform> createVaseWithFlower();

    /**
     * @brief creates a crosshair in the middle of the screen
     *
     * @param  width  screenwidth
     * @param  height screenheight
     * @return  ref_ptr conatining the geode with the crosshair
     */
    extern osg::ref_ptr<osg::Geode> createCrosshair(unsigned int width, unsigned int height);
    /**
     * @brief creates a Texture2D object with the given toonTex
     *
     * @param  filename of the toontex
     * @return ref_ptr containing the Texture2D
     */
    extern osg::ref_ptr<osg::Texture2D> createToonTex(std::string toonTex);

	/**
	* @brief creates a simple material
	*
	* 
	* @param diffuse diffuse lighting
	* @param ambient ambient lighting
	* @param specular specular lighting
	* @param shininess the shininess
	* @return the material as a osg::ref_ptr<osg::Material>
	*/

	extern osg::ref_ptr<osg::Material> createSimpleMaterial(osg::Material::Face face, const osg::Vec4& diffuse, const osg::Vec4& ambient, const osg::Vec4& specular, const double shininess);
	
	
	/**
	 * @brief return the dimension of a node (width, height, length)
	 *
	 * @param  source node, which dimension one want to know
	 * @return vec3 holding the dimensions
	 */
	extern osg::Vec3 getDimensionOfNode(osg::Node * source);
}

/*! \mainpage BrainTrain Kurzanleitung
*   For english readers:\n
*   This OSG-Demo was done as part of the computer graphics I 2014 lecture of the University of Applied Sciences and Arts Hanover.
*   This page is a short description of the project which was a requirement for passing the lecture.\n
*   Everything written here can either also be read in the source documentation or experienced by oneself by trying out this project (look for the hidden room!)\n
*   Everything here uses the GNU Public License. Use as you want, but give us credit =)!\n
*   \n
*   Das Projekt "BrainTrain" wurde im Rahmen der Vorlesung CG1 an der HS Hannover im Sommersemester 2014 erstellt.\n
*   Beteiligt waren hierbei: Jonathan Spielvogel, Marcel Felix, Gleb Ostrowski, Phillip Sauer und Sebastian Huettermann.
*
* \section Die Szene
*  
*   Ziel war es eine alte, unfertige, verfallene U-Bahn-Station zu entwerfen, in der der "Spieler" sich im First-Person-Shooter-Stil frei bewegen kann.
*   Der Spieler startet am Kopf eines kleinen Niederganges, bestehend aus Treppe mit Gelaender und Rolltreppe.
*   Direkt hinter der Startposition des Spielers befindet sich ein "geheimer Raum" (hier kann durch die Wand gelaufen werden).
*   Der untere Teil besteht aus einem einzelnen, geschwungenen Bahnsteig (mit einem Gleis).
*   Auf dem Bahnsteig selber befinden sich diverse Modelle, z.B: Kisten, ein altes Tickethaeuschen mit einer Folie darueber,
*   Oelfaesser mit einer sowjetischen Flagge darueber, einem Fliesenspiegel und viele mehr. 
*   Es gibt also viel zu entdecken!
*
*   Nicht in Blender wurde hierbei folgendes erstellt:
*   - Die Sitzbaenke wurden in OSG modelliert, materialisiert und texturiert
*   - Die Bierflaschen sowie die Vase und Blume (auf dem Tickethaeuschen) und Figuren im geheimen Raum wurden als Rotationskoerper realisiert.
*       + Die Bierflaschen wurdem zudem mit einem Partikelsystem aus Blender (zufaellig) im Raum verteilt.
*   Die primaere Farbgebung (der "Cartoon Effekt" inklusive Cel Shading und Nebel) wurde hierbei ueber eigene Shader implementiert.
*   Zudem verfuegt der Spieler ueber ein "WaffenHUD", das eine der weiteren Kamera darstellt.
*
* \section Animationen
*
*   Mehrere Dinge sind animiert:
*   Zum einen faehrt in regelmaessigen Abstaenden ein Zug das Gleis entlang.
*   Diese Animation wurde als Animation Path in OSG realisiert.
*   Und zum anderen weht die grosse, haengende Flagge "im Wind". 
*   Diese Animation wurde ueber den Shader realisiert.
*   Ebenso ueber die Shader sind die Animationen der Figuren im geheimen Raum realisiert.
*
* \section Interaktive Elemente
*
*    In der gesamten Szene kann mit einigen Elementen interagiert werden:
*   - Am rechten Ende des Bahnhofs kann von einer Kiste eine "kaputte Portalgun" mittels Links-Klick aufgehoben werden. Sobald der Spieler mehr als eine Waffe traegt kann diese mittels Scrollen des Mausrades gewechselt werden.
*   - Einige (drei) der herumliegenden Bierflaschen koennen getrunken werden. Hierzu muss der Spieler sich im geduckten Modus der Flasche naehern und kann diese - sofern der entsprechende Texthinweis erscheint - mit einem Links-Klick trinken. Hierbei handelt es sich um einen sehr starken Alkohol, der zwar schnell wirkt, seine Wirkung aber auch schnell wieder verliert. 
*       + Aus Sicherheitsgruenden darf leider nicht verraten werden, um welche Flaschen es sich handelt.
*   - Im Geheimraum am oberen Ende der Treppe stehen einige farbige Figuren. Naehert sich der Spieler diesen, so kann er mit einem Links-Klick u.a. Shader-Farben wechseln. Probieren Sie es aus!

* \section Die Steuerung
*   Die Bewegung in der Szene erfolgt im gewohnten FPS Stil. Hierbei ist sowohl eine Kollisionserkennung als auch eine "Clamp to Ground" Funktionalitaet implementiert (so dass der Spieler auf dem Boden laeuft). Die Tastaturbelegung ist hierbei die folgende:
*   - \b W Bewegung Vorwaerts, \b S Bewegung Rueckwaerts, \b A Nach links bewegen (nicht drehen), \b D Nach rechts bewegen (nicht drehen)
*   - \b Maus: Umschauen
*   - \b Mausklick links zur Interaktion (es erscheint immer ein Text der Interaktion "ankuendigt")
*   - \b Leerstaste: Springen, \b L-Shift: Sprinten, \b L-Strg: Gehen (langsamer gehen), \b X: Ducken (um z.B. an Bierflaschen heranzukommen)
*   - \b Mausrad \b scrollen: Wechseln der Waffe (sofern mehr als eine getragen wird)
*   Mit der Taste \b F kann in den Flugmodus gewechselt werden.
*   In diesem ist die Kollisionserkennung nicht mehr aktiv.
 *  Zu der o.g. Steuerung kommt Folgendes hinzu:
*   - \b Q senkrecht nach unten fliegen, \b E senkrecht nach oben fliegen
*
*   Weiteres:
*   - \b C aktiviert/deaktiviert den Polygon-Modus
*   - \b L-Shift \b + \b 1 wechselt durch die Shader-Modi, die normalerweise im Geheimraum umgeschaltet werden koennen
*
* \section Quellenverzeichnis
*
*   Sofern nicht anders dokumentiert (z.B. im Quellcode), handelt es sich bei allen Entwicklungen um Eigenentwicklungen.
*   Insbesondere sind saemtliche Modelle Eigenentwicklungen.\n
*   Texturen kommen hierbei geschlossen von\n
*   http://www.cgtextures.com/ \n
*
*   Ausnahmen sind hierbei:\n
*   UDSSR Flagge (auf den alten Oelfaessern liegend)\n
*   http://freestock.ca/soviet_union_ussr_grunge_flag_sjpg1191.jpg  \n
*   Zuletzt geprueft/gesehen: 19.06.2014\n
*
*   Flagge mit Einhorn (in der Ecke haengend)\n
*   http://wallpoper.com/images/00/24/35/71/communism-soviet_00243571.jpg \n
*   Zuletzt geprueft/gesehen: 19.06.2014\n
*/