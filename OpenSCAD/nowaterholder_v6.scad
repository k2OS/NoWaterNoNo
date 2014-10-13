// board size
// 32x50mm 

/*
 TODO:
	change mounting holes to be elongated - DONE
   Maybe slim up the PCB-compartment a bit.. and less material in the sides
	PCB needs to be redone due to new wiring of power from the battery
	total height ~20mm shorter
*/
include<batterycompartment.scad>

//translate([-14.5,23.2,20]) rotate([0,180,180]) container();

difference() {
//	translate([0,0.5,0])cube([38,19,50],center=true);
	union() {
		translate([0,0.5+7/2,0])cube([38,12,50-20],center=true);
		translate([0,1.5-11/2,0])cube([34,7,50-20],center=true);
	}
	translate([0,4.6,10.01])cube([36.5,11.2,55],center=true); // battery compartment
	translate([0,-4,4])cube([32,4,30],center=true); // PCB-compartment
	translate([0,-6.5,14])cube([26,8,50],center=true);

//	translate([0,-10.99,25.4+7])cube([39,6,50],center=true);
//	translate([0,-10.5,0.4+7]) rotate([45,0,0]) cube([39,2.15,2.15],center=true);

	translate([0,-6.25,15]) rotate([45,0,0]) cube([45,12,6],center=true);


}

// hangers
translate([-25,10,10]) {
	difference() {
		hull() {
	 		rotate([90,0,0]) cylinder(r=5,h=4,$fn=100);
	 		translate([0,0,-5]) rotate([90,0,0]) cylinder(r=5,h=4,$fn=100);
 			translate([6,-9,-20])cube([0.5,9,25]);
 		}
		hull() {
			translate([0,-2,0])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);	
			translate([0,-2,-5])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);
		}

		hull() {
			translate([0,2,0])rotate([90,0,0]) cylinder(r=2,h=20,$fn=100);
			translate([0,2,-5])rotate([90,0,0]) cylinder(r=2,h=20,$fn=100);
		}
		translate([0,2,-5])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);
	

	}
}
translate([24.26,10,10]) {
	difference() {
		hull() {
	 		rotate([90,0,0]) cylinder(r=5,h=4,$fn=100);
	 		translate([0,0,-5]) rotate([90,0,0]) cylinder(r=5,h=4,$fn=100);
 			translate([-6,-9,-20])cube([0.5,9,25]);
 		}

		hull() {
			translate([0,-2,0])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);	
			translate([0,-2,-5])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);
		}

		hull() {
			translate([0,2,0])rotate([90,0,0]) cylinder(r=2,h=20,$fn=100);
			translate([0,2,-5])rotate([90,0,0]) cylinder(r=2,h=20,$fn=100);
		}
		translate([0,2,-5])rotate([90,0,0]) cylinder(r=3,h=20,$fn=100);

	}
}
