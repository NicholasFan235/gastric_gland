
#ifndef GASTRICGLANDCELLPOPULATION_HPP_
#define GASTRICGLANDCELLPOPULATION_HPP_

#include "MeshBasedCellPopulationWithGhostNodes.hpp"

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>


template <unsigned DIM>
class GastricGlandCellPopulation : public MeshBasedCellPopulationWithGhostNodes<DIM>
{
private:
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        // This needs to be first so that MeshBasedCellPopulation::Validate() doesn't go mental.
        archive & boost::serialization::base_object<MeshBasedCellPopulationWithGhostNodes<DIM> >(*this);
    }

protected:
    double mMitosisRequiredSize;
    double mFoveolarSizeMultiplier;

public:
    GastricGlandCellPopulation(
        MutableMesh<DIM, DIM>& rMesh,
        std::vector<CellPtr>& rCells,
        const std::vector<unsigned> locationIndices,
        double mitosisRequiredSize,
        double foveolarSizeMultiplier=0.6,
        bool deleteMesh = false,
        double ghostSpringStiffness=15.0);
    
    GastricGlandCellPopulation(
        MutableMesh<DIM, DIM>& rMesh,
        double mitosisRequiredSize,
        double foveolarSizeMultiplier=0.6,
        double ghostSpringStiffness=15.0);

    virtual ~GastricGlandCellPopulation();

    virtual bool IsRoomToDivide(CellPtr pCell) override;

    virtual double GetRestLength(unsigned indexA, unsigned indexB) override;

    double GetMitosisRequiredSize() const;
    void SetMitosisRequiredSize(double size);

    double GetFoveolarSizeMultiplier() const;
    void SetFoveolarSizeMultiplier(double mul);

    void OutputCellPopulationParameters(out_stream& rParamsFile);

};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(GastricGlandCellPopulation)

namespace boost
{
    namespace serialization
    {
        /**
         * Serialize information required to construct a MeshBasedCellPopulationWithGhostNodes.
         */
        template<class Archive, unsigned DIM>
        inline void save_construct_data(
            Archive & ar, const GastricGlandCellPopulation<DIM> * t, const unsigned int file_version)
        {
            // Save data required to construct instance
            const MutableMesh<DIM,DIM>* p_mesh = &(t->rGetMesh());
            ar << p_mesh;
            ar << t->GetMitosisRequiredSize();
            ar << t->GetFoveolarSizeMultiplier();
        }

        /**
         * De-serialize constructor parameters and initialise a MeshBasedCellPopulationWithGhostNodes.
         * Loads the mesh from separate files.
         */
        template<class Archive, unsigned DIM>
        inline void load_construct_data(
            Archive & ar, GastricGlandCellPopulation<DIM> * t, const unsigned int file_version)
        {
            // Retrieve data from archive required to construct new instance
            MutableMesh<DIM,DIM>* p_mesh;
            ar >> p_mesh;
            double mitosisRequiredSize;
            ar >> mitosisRequiredSize;
            double foveolarSizeMultiplier;
            ar >> foveolarSizeMultiplier;

            // Invoke inplace constructor to initialise instance
            ::new(t)GastricGlandCellPopulation<DIM>(*p_mesh, mitosisRequiredSize, foveolarSizeMultiplier);
        }

    } // namespace serialization
} // namespace boost

#endif // GASTRICGLANDCELLPOPULATION_HPP_
