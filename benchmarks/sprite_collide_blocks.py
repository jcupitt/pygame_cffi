# Sample Python/Pygame Programs
# Simpson College Computer Science
# http://programarcadegames.com/
# http://simpson.edu/computer-science/

# Explanation video: http://youtu.be/4W2AqUetBi4

import pygame
import random

from base import Benchmark


# Define some colors
black    = (   0,   0,   0)
white    = ( 255, 255, 255)
red      = ( 255,   0,   0)

# This class represents the ball
# It derives from the "Sprite" class in Pygame
class Block(pygame.sprite.Sprite):

    # Constructor. Pass in the color of the block,
    # and its x and y position
    def __init__(self, color, width, height):
        # Call the parent class (Sprite) constructor
        pygame.sprite.Sprite.__init__(self)

        # Create an image of the block, and fill it with a color.
        # This could also be an image loaded from the disk.
        self.image = pygame.Surface([width, height])
        self.image.fill(color)

        # Fetch the rectangle object that has the dimensions of the image
        # image.
        # Update the position of this object by setting the values
        # of rect.x and rect.y
        self.rect = self.image.get_rect()


class SpriteCollideBenchmark(Benchmark):

    def setUp(self):
        # Initialize Pygame
        pygame.init()

        # Set the height and width of the screen
        screen_width=700
        screen_height=400
        self.screen=pygame.display.set_mode([screen_width,screen_height])

        # This is a list of 'sprites.' Each block in the program is
        # added to this list. The list is managed by a class called 'Group.'
        self.block_list = pygame.sprite.Group()

        # This is a list of every sprite. All blocks and the player block as well.
        self.all_sprites_list = pygame.sprite.Group()

        for i in range(50):
            # This represents a block
            block = Block(black, 20, 15)

            # Set a random location for the block
            block.rect.x = random.randrange(screen_width)
            block.rect.y = random.randrange(screen_height)

            # Add the block to the list of objects
            self.block_list.add(block)
            self.all_sprites_list.add(block)



        # Create a red player block
        self.player = Block(red, 20, 15)
        self.all_sprites_list.add(self.player)

        self.score = 0

    def tearDown(self):
        pygame.quit()

    def main(self, clock):
        done = False

        # -------- Main Program Loop -----------
        while done==False:
            for event in pygame.event.get(): # User did something
                if event.type == pygame.QUIT: # If user clicked close
                    done = True # Flag that we are done so we exit this loop

            # Clear the screen
            self.screen.fill(white)

            # Get the current mouse position. This returns the position
            # as a list of two numbers.
            pos = pygame.mouse.get_pos()

            # Fetch the x and y out of the list,
               # just like we'd fetch letters out of a string.
            # Set the player object to the mouse location
            self.player.rect.x = pos[0]
            self.player.rect.y = pos[1]

            # See if the player block has collided with anything.
            blocks_hit_list = pygame.sprite.spritecollide(self.player, self.block_list, False)

            # Check the list of collisions.
            for block in blocks_hit_list:
                self.score +=1

            # Draw all the spites
            self.all_sprites_list.draw(self.screen)

            # Go ahead and update the screen with what we've drawn.
            pygame.display.flip()

            clock.tick()


benchmark_class = SpriteCollideBenchmark
