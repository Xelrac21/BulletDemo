# BulletDemo
Basic program running Bullet Physics with OpenGl to render

Used as a demo program for a presentation to students on the basics of physics engines, Bullet Physics in particular.

Features things such as:
- world creation
- adding the ground object (shown in the presentation first as a rectangle and then as a plane to showcase the difference between the rendered graphics vs the physics body)
- adding bodies (shapes) onto the world
- combining those shapes to form a compound shape
- using that compound shape to form a cannon vehicle
- moving the cannon vehicle around with an over the shoulder third person camera
- rotating and aiming the cannon
- shooting sphere objects from the cannon
- randomly generated obstacle course made up of rectangles, which showcase both static and dynamic bodies
- in the presentation, it initially simulates the physics world discretely but a sphere object is purposely shown to clip through an obstacle, and is then changed to a continuous simulation to show the difference
