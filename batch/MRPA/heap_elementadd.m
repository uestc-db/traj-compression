function [heaps,heapind,invind] = heap_elementadd(heaps,heapind,addval,addind,invind,heapsize)
%invind is current pos of 1:l point in heaps
if nargout>2
    b_inv = 1;
else
    b_inv = 0;
end
if nargin<6
    heapsize = length(heaps);
end


l = heapsize + 1;
heaps(l) = addval;
heapind(l) = addind;

if b_inv
   invind(addind)= l;
end

curnode = l;
while (curnode>1)
    parnode = fix(curnode/2);
    if heaps(parnode)>heaps(curnode)%FOR MIN-heaps
        %swap value
        tmp = heaps(parnode);
        heaps(parnode) = heaps(curnode);
        heaps(curnode) = tmp;
        %swap heapind
        tmp = heapind(parnode);
        heapind(parnode) = heapind(curnode);
        heapind(curnode) = tmp;
        if b_inv
            %update invind
            invind(heapind(curnode))= curnode;
            invind(heapind(parnode))= parnode;
        end
        
        curnode = parnode;
    else
        break;
    end
end