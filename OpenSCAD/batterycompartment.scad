



//rotate([-90, 0, 0])
//container();




$fn=100;



//battery

cellX	= 34.1 +0.5;
cellY	= 05.8;
cellZ	= 53.1 +1;

recessX	= 30.1;
recessY	= 02.4;
recessZ	= 01.8;


module battery()
{
	difference()
	{
		hull()
		{
			cylinder(r=cellY/2, h=cellZ);
			
			translate([cellX-cellY, 0, 0])
			cylinder(r=cellY/2, h=cellZ);
			
			translate([-cellY/2, -cellY/2-20, 0])
			cube([cellX, cellY/2+20, cellZ]);
		}
		
		//corner pits
		translate([(cellX-cellY)/2, 0, 0])
		union()
		{
			translate([-recessX/2-cellY, cellY/2-recessY, -cellY+recessZ])
			cube([cellY, cellY, cellY]);
			
			translate([recessX/2, cellY/2-recessY, -cellY+recessZ])
			cube([cellY, cellY, cellY]);
		}
	}
}





//connector

connX	= 9.3;
connY	= 6.9;
connZ	= 4.9;
connPos	= 1.5;

module connector()
{
//	translate([0, -connPos, -connZ])
	translate([0, connPos-connY, -connZ])
	cube([connX, connY, connZ+0.01]);
}





//container

wall	= 1;

module container()
{
	difference()
	{
		union()
		{
			difference()
			{
				translate([-cellY/2-wall, -connY+connPos+0.01, -connZ-wall])
				cube([cellX+2*wall, connY+connPos+wall-0.01, connZ+cellZ+2*wall]);
				
				union()
				{
					battery();
					connector();
				}
			}
			
			translate([-cellY/2+cellX/2+4, -cellY/2, 0])
			rotate([0, 0, 180])
			rotate([-45, 0, 0])
			cube([8, recessZ, recessZ]);
			
			translate([-cellY/2+cellX+wall, -cellY/2, cellZ])
			rotate([0, 0, 180])
			rotate([-45, 0, 0])
			cube([cellX+2*wall, recessZ/2, recessZ/2]);
		}
		
		translate([-cellY/2+cellX/2, connPos-connY-1, cellZ+wall])
		rotate([-90, 0, 0])
		cylinder(r=10, h=connY-connPos+cellY/2+wall+2);
	}
}
