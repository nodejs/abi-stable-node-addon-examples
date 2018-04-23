const addon = require('./build/Release/inheritance.node');

// -----------------------------------------------------------------------------

console.log('Subclass');

let x = new addon.Subclass();

console.log('Construction');

console.log(JSON.stringify(x, null, 4));

x.subclassMethod();

console.log('Subclass method');

console.log(JSON.stringify(x, null, 4));

addon.Subclass.subclassStaticMethod(x);

console.log('Subclass static method');

console.log(JSON.stringify(x, null, 4));

x.superclassOverriddenMethod();

console.log('Superclass overridden method');

console.log(JSON.stringify(x, null, 4));

addon.Subclass.superclassOverriddenStaticMethod(x);

console.log('Superclass overridden static method');

console.log(JSON.stringify(x, null, 4));

x.superclassChainedMethod();

console.log('Superclass chained method');

console.log(JSON.stringify(x, null, 4));

addon.Subclass.superclassChainedStaticMethod(x);

console.log('Superclass chained static method');

console.log(JSON.stringify(x, null, 4));

// -----------------------------------------------------------------------------

console.log('Superclass');

x = new addon.Superclass();

console.log('Construction');

console.log(JSON.stringify(x, null, 4));

x.superclassMethod();

console.log('Superclass method');

console.log(JSON.stringify(x, null, 4));

addon.Superclass.superclassStaticMethod(x);

console.log('Superclass static method');

console.log(JSON.stringify(x, null, 4));

x.superclassOverriddenMethod();

console.log('Superclass overridden method');

console.log(JSON.stringify(x, null, 4));

addon.Superclass.superclassOverriddenStaticMethod(x);

console.log('Superclass overridden static method');

console.log(JSON.stringify(x, null, 4));

x.superclassChainedMethod();

console.log('Superclass chained method');

console.log(JSON.stringify(x, null, 4));

addon.Superclass.superclassChainedStaticMethod(x);

console.log('Superclass chained static method');

console.log(JSON.stringify(x, null, 4));
