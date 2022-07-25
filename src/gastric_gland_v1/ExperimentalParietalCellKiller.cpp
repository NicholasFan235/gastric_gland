#include "ExperimentalParietalCellKiller.hpp"

#include "CellLabel.hpp"
#include "AbstractCentreBasedCellPopulation.hpp"
#include "DifferentiatedCellProliferativeType.hpp"
#include "NeckCellProliferativeType.hpp"
#include "Exception.hpp"

template <unsigned DIM>
ExperimentalParietalCellKiller<DIM>::ExperimentalParietalCellKiller(
        AbstractCellPopulation<DIM>* pCellPopulation,
        double death_chance,
        double activation_time,
        bool has_activated) :
    AbstractCellKiller<DIM>(pCellPopulation),
    m_deathChance(death_chance),
    m_activationTime(activation_time),
    m_hasActivated(has_activated)
{}

template <unsigned DIM>
void ExperimentalParietalCellKiller<DIM>::CheckAndLabelCellsForApoptosisOrDeath()
{
    if (m_hasActivated) return;
    double currentTime = SimulationTime::Instance()->GetTime();
    if (currentTime < m_activationTime) return;
    m_hasActivated = true;

    RandomNumberGenerator* p_gen = RandomNumberGenerator::Instance();

    for (typename AbstractCellPopulation<DIM>::Iterator cell_iter = this->mpCellPopulation->Begin(); cell_iter != this->mpCellPopulation->End(); ++cell_iter)
    {
        CellPtr pCell = *cell_iter;
        if (!pCell->GetCellProliferativeType()->IsType<NeckCellProliferativeType>())
            continue;
        if (p_gen->ranf() <= m_deathChance)
        {
            pCell->Kill();
        }
    }
}

template <unsigned DIM>
double ExperimentalParietalCellKiller<DIM>::GetDeathChance() const
{
    return m_deathChance;
}

template <unsigned DIM>
void ExperimentalParietalCellKiller<DIM>::SetDeathChance(double death_chance)
{
    m_deathChance = death_chance;
}

template <unsigned DIM>
double ExperimentalParietalCellKiller<DIM>::GetActivationTime() const
{
    return m_activationTime;
}

template <unsigned DIM>
void ExperimentalParietalCellKiller<DIM>::SetActivationTime(double activation_time)
{
    m_activationTime = activation_time;
}

template <unsigned DIM>
bool ExperimentalParietalCellKiller<DIM>::HasActivated() const
{
    return m_hasActivated;
}

template <unsigned DIM>
void ExperimentalParietalCellKiller<DIM>::OutputCellKillerParameters(out_stream& rParamsFile)
{
    *rParamsFile << "\t\t\t<DeathChance>" << m_deathChance << "</DeathChance>\n";
    *rParamsFile << "\t\t\t<ActivationTime>" << m_activationTime << "</ActivationTime>\n";

    AbstractCellKiller<DIM>::OutputCellKillerParameters(rParamsFile);
}

template class ExperimentalParietalCellKiller<2u>;

#include "SerializationExportWrapperForCpp.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(ExperimentalParietalCellKiller)
