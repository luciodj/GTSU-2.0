#
# ScreenCapture visualizer 
#
import sys, pygame

# init graphics environment
pygame.init()
Clock = pygame.time.Clock()

size = width, height = 320, 240

# define a screen surface
screen = pygame.display.set_mode( size)
#pygame.scrap.init()


# main loop
while 1:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
	    #pygame.scrap.put (SCRAP_BMP, screen)
            print Clock.get_fps()
	    print ( r, g, b)
            sys.exit()

    # paint image
    Clock.tick( 1)
    screen.fill( (0, 0, 0))

    if len(sys.argv) == 0:
	filename = "SCREEN.SCR"
    else:
        filename = sys.argv[1]
    try:
        f = open( filename, "rb")
        for j in range( 240):
            for i in range( 320):
                bytel = ord( f.read(1))
                byteh = ord( f.read(1))
                b = ((bytel & 0x1f) << 3) + 7
                r = byteh & 0xF8 + 7
                g = ((bytel & 0xe0) >> 3) + ((byteh & 0x7) << 5) + 3

                pygame.draw.rect( screen, ( r, g, b), ( i, j, 2, 2), 0)

    finally:
        f.close()

    pygame.display.flip()
