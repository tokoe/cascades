#ifndef FILTERPROXYDATAMODELTEST_HPP
#define FILTERPROXYDATAMODELTEST_HPP

#include <QObject>

class FilterProxyDataModelTest : public QObject
{
    Q_OBJECT

private slots:
    void testEmptySourceModel();
    void testSimpleFilter();
    void testInsertNonMatchToSourceModel1();
    void testInsertNonMatchToSourceModel2();
    void testInsertMatchToSourceModel1();
    void testInsertMatchToSourceModel2();
    void testRemoveNonMatchFromSourceModel1();
    void testRemoveNonMatchFromSourceModel2();
    void testRemoveMatchFromSourceModel1();
    void testRemoveMatchFromSourceModel2();
    void testRemoveMatchFromSourceModel3();
    void testUpdateNonMatchInSourceModel1();
    void testUpdateNonMatchInSourceModel2();
    void testUpdateMatchInSourceModel1();
    void testUpdateMatchInSourceModel2();
    void testUpdateMatchInSourceModel3();
};

#endif
