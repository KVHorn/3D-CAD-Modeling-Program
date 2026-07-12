#ifndef INFINITYFORMAT_H
#define INFINITYFORMAT_H

#include <QString>

class PartModel;
class DocumentSettings;

// The .infinity native format: the feature history plus document settings,
// written down as versioned JSON. Loading = reconstruct features via the
// FeatureFactory and replay. Schema documented in docs/FILE_FORMAT.md.
namespace InfinityFormat
{
    // Returns true on success; errorMessage is filled on failure.
    bool save(const PartModel& model, const DocumentSettings& settings,
              const QString& filePath, QString* errorMessage = nullptr);

    // Populates model (replacing its contents) and settings.
    bool load(PartModel& model, DocumentSettings& settings,
              const QString& filePath, QString* errorMessage = nullptr);
}

#endif // INFINITYFORMAT_H
