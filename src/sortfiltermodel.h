#ifndef SORTFILTERMODEL_H
#define SORTFILTERMODEL_H

#include <QSortFilterProxyModel>

class SortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterModel(QObject *parent = 0);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    
signals:
    
public slots:
    
};

#endif // SORTFILTERMODEL_H
