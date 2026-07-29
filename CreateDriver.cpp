

#include "CreateDriver.h"

void CreateDriver::Create(Data &data) {

    for(int i=0;i<data.GetDepotCount();i++){
        Node *dep = data.GetDepot(i);

        for(int k=0;k<data.GetDriverCount();k++){

            Driver *d = data.GetDriver(k);

            Node dep1;
            dep1.id = data.GetNodeCount();
            dep1.no = dep->no;
            dep1.x = dep->x;
            dep1.y = dep->y;
            dep1.distID = dep->distID;

            Node dep2(dep1);
            dep2.id = dep1.id +1;
            dep1.type = Parameters::TypeNode::START_LINK;
            dep2.type = Parameters::TypeNode::END_LINK;

            d->StartNodeID = dep1.id;
            d->EndNodeID = dep2.id;
            d->distID = dep->distID;
            data.AddNode(dep1);
            data.AddNode(dep2);
        }

    }

}
