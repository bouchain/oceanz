#include "mainwindow.hh"

#include "tgafile.hh"
#include <string>

#include <cmath>


MainWindow::MainWindow( QWidget *parent )
    : QGLWidget( parent ) {
    _timer = new QTimer();
    _wm = new WorldMap( 1024, 1024 );

    this->setAttribute( Qt::WA_QuitOnClose, true );
    xpan = ypan = 0;
    screen_surface = 0;

    loadLandSprites();

    this->setMinimumSize( sizeHint() );
}

MainWindow::~MainWindow() {
    delete _wm;
    delete _timer;

    delete land_sprite;
    delete water_sprite;

    deleteLandSprites();
}

QSize MainWindow::sizeHint() {
    return QSize( 800, 600 );
}

void
MainWindow::setWorldMap( WorldMap * map ) {
    if( _wm ) delete _wm;
    _wm = map;
}

void
MainWindow::initializeGL() {
    // SET UP for SHADERS goes HERE

    // SET UP for SCREEN TEXTURE is HERE:
    glGenTextures( 1, &screen_texture );
    glBindTexture( GL_TEXTURE_2D, screen_texture );
    glTexEnvi( GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glEnable( GL_TEXTURE_2D );
    // glEnable( GL_BLEND );
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // THIS SPRITE IS TEMPORARY:
//    TGAFile land_tga( "tileframes/land_00.tga" );
//    TGAFile water_tga( "tileframes/water_00.tga" );

//    land_sprite = new Surface( land_tga );
//    water_sprite = new Surface( water_tga );

    // try stb_image lib
    land_sprite = new Surface( "tileframes/land_00.tga" );
    water_sprite = new Surface( "tileframes/water_00.tga" );
    city_sprite = new Surface( "tileframes/city_00.tga" );
    around_city_sprite = new Surface( "tileframes/around_city.tga" );
    around_city_water_sprite = new Surface( "tileframes/around_city_water.tga" );
}

#ifndef PI
#define PI static_cast< float >( M_PI )
#endif

void
MainWindow::resizeGL( int w, int h ) {
    // Re-create surface:
    delete screen_surface;
    screen_surface = new Surface( w, h );
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0., w, h, 0., 0., 10. );
    updateGL();
}

#define GL_BGRA 0x80E1

void
MainWindow::updateGL() {
//    glClearColor( 0.f, 0.f, 0.f, 1.f );
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // glTranslatef( -xpan * 2 * rad * cos( PI / 6 ) + ypan* rad * cos( PI/6 ),
    //               -ypan * ( rad + rad * sin( PI/6 ) ),
    //               0 );
    screen_surface->clear( 0, 0, 0, 0xFF );
    drawWorldMap();

    // THIS is the FULLSCREEN QUAD:
    glBindTexture( GL_TEXTURE_2D, screen_texture );

    glTexImage2D( GL_TEXTURE_2D, 0,
                  GL_RGBA,
                  width(), height(), 0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  *screen_surface );
    glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 ); glVertex2i( 0, 0 );
    glTexCoord2i( 0, 1 ); glVertex2i( 0, height() );
    glTexCoord2i( 1, 1 ); glVertex2i( width(), height() );
    glTexCoord2i( 1, 0 ); glVertex2i( width(), 0 );
    glEnd();
    swapBuffers();
}

void
MainWindow::drawWorldMap() {
    for( int x = xpan - 1 - width()/32 - height()/60;
         x < xpan + 1 + width()/32 + height()/60; ++x ) {
        for( int y = ypan - 1 - height()/30; y < ypan + 2 + height()/30; ++y ) {

            if( x < 0 || y < 0 || x >= _wm->sizeX() || y >= _wm->sizeY() )
                continue;

            int xa = 8*( 2*( x - xpan ) - ( y - ypan ) ) + width()/2 - 8;
            int ya = 15*( y - ypan ) + height()/2 - 7;

            if( _wm->region( x, y )->is( LAND ) ) {
                if( _wm->cityLayer()->insideCity( x, y ) != 0 ) {
                    screen_surface->paint( *city_sprite, xa, ya );
                } else if ( _wm->cityLayer()->aroundCity( x, y ) ) { 
                    screen_surface->paint( *around_city_sprite, xa, ya );
                } else
                screen_surface->paint( *land_sprite, xa, ya );
            }
            else if( _wm->cityLayer()->aroundCity( x, y ) )
                screen_surface->paint( *around_city_water_sprite, xa, ya );
            else
                screen_surface->paint( *water_sprite, xa, ya );
        }
    }            
}

void
MainWindow::drawCities() {
//    float rad = 10;
//    for( auto it  = _wm->cityLayer()->cities().begin();
//              it != _wm->cityLayer()->cities().end();
//              it++ ) {
//
//        glPushMatrix();
//
//        glTranslatef( ( 2*it->second.x() - it->second.y() ) * rad * cos( PI/ 6 ),
//                      it->second.y() * ( rad + rad * sin( PI/6 ) ),
//                      0.f);
//        drawCity( it->first );
//
//        glPopMatrix();
//    }
}

void
MainWindow::drawCity( City* c ) {
    c = c;
//    float rad = 10.f;
//    std::list< Position > list = c->positions();
//    for( auto it  = list.begin(); it != list.end(); it++ ) {
//        glTranslatef( ( 2*it->x() - it->y() ) * rad * cos( PI/ 6 ),
//                      it->y() * ( rad + rad * sin( PI/6 ) ),
//                      0.f);
//        drawTile( 1.f, 1.f, 0.f );
//    }
}

void
MainWindow::drawTile( float r, float g, float b ) {
    float rad = 10.f;
    glColor3f( r, g, b );
    glBegin( GL_POLYGON );
        for( int k = 0; k < 6; k++ )
        glVertex2f( rad*cos( PI/6 + k*PI/3 ),
                    rad*sin( PI/6 + k*PI/3 ) );
    glEnd();
}

void
MainWindow::drawShip() {
}

void
MainWindow::drawControlPanel() {
}

void
MainWindow::keyPressEvent( QKeyEvent *e ) {
    switch( e->key() ) {
        case Qt::Key_Escape:
            this->close();
            break;
        case Qt::Key_U:
            this->updateGL();
            break;
        case Qt::Key_L:
        case Qt::Key_Right:
            xpan += 4;
            updateGL();
            break;
        case Qt::Key_H:
        case Qt::Key_Left:
            xpan -= 4;
            updateGL();
            break;
        case Qt::Key_K:
        case Qt::Key_Up:
            ypan -= 4;
            xpan -= 2;
            updateGL();
            break;
        case Qt::Key_J:
        case Qt::Key_Down:
            ypan += 4;
            xpan += 2;
            updateGL();
            break;
    }
}

void
MainWindow::loadLandSprites() {
//    std::string land_sprite_name;
//    land_sprites = new Surface*[32];
//    for( int i = 0; i < 32; i++ )
//        land_sprites[ i ] = 0;
//
//    land_sprites[ 0 ] = new Surface( "tileframes/land_00.tga" );
//    land_sprites[ 31] = new Surface( "tileframes/land_60.tga" );
//
//    for( int i = 1; i < 5; i++ )
//        for( int j = 0; j < 6; j++ ) {
//            land_sprite_name += "tileframes/land_" + i;
//            land_sprite_name += j + ".tga";
//            land_sprites[ (i-1)*6 + j + 1 ] = new Surface( land_sprite_name );
//        }
}

void
MainWindow::deleteLandSprites() {
//    for( int i = 0; i < 32; i++ )
//        if( land_sprites[ i ] )
//            delete land_sprites[ i ];
//    delete[] land_sprites;
}
