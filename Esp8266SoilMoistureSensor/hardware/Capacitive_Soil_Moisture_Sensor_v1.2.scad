/*
 width = x
 depth = y
 height = z
*/

/* General model parameter. Change according to you printer and needs. */
createUpperSTL = true;
createLowerSTL = true;
PCBWiggleRoom = 1.0;
HousingWiggleRoom = 0.3;

HousingWidth = 29.0;
HousingDepth = HousingWidth;
HousingHeight = 12;
HousingCornerRadius = 5.0;
HousingWallWidth = 2.0; /* Top and buttom wall width */
HousingPCBHeightOffset = 1;

/* Sensor related parameter */
PCBWidth = 23 + PCBWiggleRoom;
PCBDepth = 98.0 + PCBWiggleRoom;
PCBHeight = 1.0 + PCBWiggleRoom;
PCBNotch_y = 21.0 + PCBWiggleRoom; /* center of notch */
PCBNotchWidth = 2.0 - PCBWiggleRoom;
PCBNotchDepth = 4.0 - PCBWiggleRoom;
PCBNotchHeight = 2.0;
PCBConnectorWidth = 10.0 + PCBWiggleRoom;
PCBConnectorDepth = 10.0 + PCBWiggleRoom;
PCBConnectorHeight = 6.0 + PCBWiggleRoom;
PCBConnectorPinsWidth = PCBWidth;
PCBConnectorPinsDepth = 7.0;
PCBConnectorPinsHeight = 2.0;
PCBConnectorPins_y = 5.0; /* center of pins */

if (createUpperSTL)
{
    translate([0, 0, 5])
    color("LawnGreen")
    sensorHousingUpper($fn=100); 
}

if (createLowerSTL)
{
    color("DeepSkyBlue")
    sensorHousingLower($fn=100);
}

//translate([0, -PCBDepth/2 + HousingDepth/2 - HousingWallWidth, HousingWallWidth + HousingPCBHeightOffset])
//    capacitiveSoilSensor();


module sensorHousingUpper()
{
    difference()
    {
        translate ([0, 0, HousingWallWidth + HousingPCBHeightOffset + PCBHeight])
        linear_extrude(height = HousingHeight - HousingWallWidth - HousingPCBHeightOffset - PCBHeight)
        hull()
        {
            translate([(HousingWidth/2 - HousingCornerRadius), (HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([-(HousingWidth/2 - HousingCornerRadius), (HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([(HousingWidth/2 - HousingCornerRadius), -(HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([-(HousingWidth/2 - HousingCornerRadius), -(HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
        }
        
        translate([0, -PCBDepth/2 + HousingDepth/2 - HousingWallWidth, HousingWallWidth + HousingPCBHeightOffset])
            capacitiveSoilSensor();
        
        SpaceAbovePCB = HousingHeight - 2 * HousingWallWidth - HousingPCBHeightOffset - PCBHeight;
        translate ([0, HousingWallWidth, (SpaceAbovePCB)/2 + HousingWallWidth + HousingPCBHeightOffset + PCBHeight])
            cube([PCBWidth - 2 * HousingWallWidth, HousingDepth, SpaceAbovePCB], center = true);
        translate([(HousingWidth + HousingWallWidth)/2, 0, HousingWallWidth])
            cube([HousingWallWidth, HousingDepth, SpaceAbovePCB], center = true);

        translate([(HousingWidth - PCBWidth)/4 + PCBWidth/2, - 2 * HousingWiggleRoom, -SpaceAbovePCB/2 + HousingHeight - HousingWallWidth])
            cube([(HousingWidth - PCBWidth)/2 + HousingWiggleRoom, HousingDepth, SpaceAbovePCB], center = true);
        translate([-((HousingWidth - PCBWidth)/4 + PCBWidth/2), -  2 * HousingWiggleRoom, -SpaceAbovePCB/2 + HousingHeight - HousingWallWidth])
            cube([(HousingWidth - PCBWidth)/2 + HousingWiggleRoom, HousingDepth, SpaceAbovePCB], center = true);
        translate([0, (HousingDepth - HousingWallWidth)/2,  -SpaceAbovePCB/2 + HousingHeight - HousingWallWidth])
            cube([HousingWidth, HousingWallWidth + HousingWiggleRoom, SpaceAbovePCB], center = true);
    }
}
            
module sensorHousingLower()
{
    difference()
    {
        linear_extrude(height = HousingHeight - HousingWallWidth)
        hull()
        {
            translate([(HousingWidth/2 - HousingCornerRadius), (HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([-(HousingWidth/2 - HousingCornerRadius), (HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([(HousingWidth/2 - HousingCornerRadius), -(HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
            translate([-(HousingWidth/2 - HousingCornerRadius), -(HousingDepth/2 - HousingCornerRadius), 0])
                circle(r = HousingCornerRadius);
        }
        
        translate([0, -PCBDepth/2 + HousingDepth/2 - HousingWallWidth, HousingWallWidth + HousingPCBHeightOffset])
          capacitiveSoilSensor();
        
        SpaceAbovePCB = HousingHeight - 2 * HousingWallWidth - HousingPCBHeightOffset - PCBHeight;
        translate ([0, -HousingWallWidth, (SpaceAbovePCB)/2 + HousingWallWidth + HousingPCBHeightOffset + PCBHeight])
          cube([PCBWidth, HousingDepth, SpaceAbovePCB], center = true);
    }
}



module capacitiveSoilSensor()
{
    difference()
    {
        /* PCB and connector */
        union()
        {
            translate([0, 0, PCBHeight/2])
            {
                cube ([PCBWidth, PCBDepth, PCBHeight], center=true);
                translate([0, PCBDepth/2, PCBConnectorHeight/2 + PCBHeight/2 ])
                    cube ([PCBConnectorWidth, PCBConnectorDepth, PCBConnectorHeight], center=true);
                translate([0, PCBDepth/2 - PCBConnectorPins_y, -PCBConnectorPinsHeight/2 - PCBHeight/2])
                    cube([PCBConnectorPinsWidth, PCBConnectorPinsDepth, PCBConnectorPinsHeight], center =true);
            }
        }
        translate([PCBWidth/2 - PCBNotchWidth/2, PCBDepth/2 - PCBNotch_y, PCBHeight/2])
        {
            cube([PCBNotchWidth, PCBNotchDepth, PCBNotchHeight], center=true);
        }
        translate([- (PCBWidth/2 - PCBNotchWidth/2), PCBDepth/2 - PCBNotch_y, PCBHeight/2])
        {
            cube([PCBNotchWidth, PCBNotchDepth, PCBNotchHeight], center=true);
        }
    }
}
