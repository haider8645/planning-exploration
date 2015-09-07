#ifndef EXPLORATION_EXPLORATION_PLANNER_H
#define EXPLORATION_EXPLORATION_PLANNER_H

#include <exploration/ExplorationPlannerTypes.hpp>
#include <base/samples/RigidBodyState.hpp>
#include <envire/Core.hpp>
#include <envire/maps/MLSGrid.hpp>
#include <envire/maps/TraversabilityGrid.hpp>

namespace exploration
{    
	class Planner
	{
	public:
		Planner();
		~Planner();
		
		/** Returns the result of the last operation */
		Status getStatus() const {return mStatus;}
		
		/** Returns the result message of the last operation */
		const char* getStatusMessage() const {return mStatusMessage;}
		
		/** Returns a list of reachable frontier cells ordered by distance to 'start' */
		PointList getFrontierCells(GridMap* map, GridPoint start, bool stopAtFirst = false);
		
		/** Returns a set of connected frontiers ordered by distance to 'start' */
		FrontierList getFrontiers(GridMap* map, GridPoint start);
		
		/** Returns the number of reachable frontier cells after detection */
		unsigned int getFrontierCellCount() const {return mFrontierCellCount;}
		
		/** Initializes a new coverage map from a given grid map
		 *  All cells marked 'free' in the map are marked 'uncovered' in the coverage map */
		void initCoverageMap(GridMap* map);
		
		/** Set a cell to a given value (used to add new obstacles)
		 *  -1 : uncovered
		 *   0 : covered
		 *   1 : obstacle */
		void setCoverageMap(PointList points, char value = 1);
		
		/** Cover all cells within sensor footprint from the given pose. Uses the 'willBeExplored' function */
		void addReading(Pose p);
                
		
		/** Get all cells that are still uncovered */
		PointList getUnexploredCells() const;
		
		/** Add a sensor footprint to the internal sensor field */
		void addSensor(Polygon p) {mSensorField.push_back(p);}
		
		/** Coordinate-transgform the given polygon to pose */ 
		Polygon transformPolygon(Polygon polygon, Pose pose);
		
		/** Get reachable Frontiers in coverage map from given pose **/
		FrontierList getCoverageFrontiers(Pose start);
		
		/** (Experimental!) 
		 * Currently returns nearest frontier cell in coverage map */
		Pose getCoverageTarget(Pose start);
                
        const GridMap& getCoverageMap() const;

        void setMinGoalDistance(double value) {min_goal_distance = value;}

        double getMinGoalDistance() const {return min_goal_distance;}

        /**
         * Get the point with least angular difference to robotpose. Uses compare-function for sorting.
         * If calculate_worst_driveability is set to true the robot length and with have to be specified as well.
         * In this case obstacle poses are ignored and the worst driveability is used
         * within the cost calculations.
         * \param robotPose Expects local coordinates in meter.
         */
        std::vector<base::samples::RigidBodyState> getCheapest(std::vector<base::Vector3d> &pts, 
                base::samples::RigidBodyState &robotPose,
                bool calculate_worst_driveability = false,
                double robot_length_x = 0,
                double robot_width_y = 0);

        /**contains extracted travGrid **/
        envire::TraversabilityGrid* mTraversability;

        /**copy of exploremap as TravGrid. Used if the GridMap needs to be resized, thus transformed**/
        envire::TraversabilityGrid* mCoverageMapAsTravGrid;

        /**takes a GridMap and turns it into a TraversabilityGrid with the attributes of the given TravGrid**/
        envire::TraversabilityGrid* coverageMapToTravGrid(const GridMap& mapToBeTranslated, envire::TraversabilityGrid& traversability);
        
        /**
         * Clears the complete map / sets everything to UNKNOWN.
         * Receiving the next trav map will reset all OBSTACLES again.
         */
        inline void clearCoverageMap() {
            mCoverageMap->clearData();
        }
       
	private:
		PointList getNeighbors(GridPoint p, bool diagonal = false) const;
		PointList getFrontier(GridMap* map, GridMap* plan, GridPoint start);
		bool isFrontierCell(GridMap* map, GridPoint point) const;
		bool pointInPolygon(FloatPoint point, Polygon polygon) const;
		bool isVisible(FloatPoint point, Pose pose) const;
		SensorField transformSensorField(Pose pose);
                
        /** returns all cells within the sensor footprint from the given pose */
        PointList willBeExplored(Pose p);

		Status mStatus;
		char mStatusMessage[500];
		
		unsigned int mFrontierCellCount;
		char mFrontierCount;
		
		GridMap* mCoverageMap;
		SensorField mSensorField;
                
        double min_goal_distance;
        
        /**
         * Maps the passed angle to [0, 2*PI)
         */
        double map0to2pi(double angle_rad);
	};
}

#endif
