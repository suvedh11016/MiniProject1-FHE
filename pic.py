from graphviz import Digraph

def draw_flowchart():
    dot = Digraph()

    # Input
    dot.node('A', 'Input: 129*8 + 7*4 + 5*2')

    # Processing steps
    dot.node('B', 'Expand: 129*8 → 10000001\n(use direct multiplication)')
    dot.node('C', 'Expand: 7*4 → strength reduction: 111')
    dot.node('D', 'Expand: 5*2 → strength reduction: 101')
    dot.node('E', 'Construct expr → (((129*8 + 4) + (2*4 + 4*4)) + (2 + 4*2))')

    # Encryption
    dot.node('F', 'Encrypt each value\nNoise Budget: 145')
    dot.node('G', 'Encrypted results after *\nNoise Budget: 113')
    dot.node('H', 'Encrypted results after +\nNoise Budget: 112→111')

    # Decryption
    dot.node('I', 'Decrypt result → 1070')

    # Connections
    dot.edges(['AB', 'AC', 'AD'])
    dot.edge('B', 'E')
    dot.edge('C', 'E')
    dot.edge('D', 'E')
    dot.edge('E', 'F')
    dot.edge('F', 'G')
    dot.edge('G', 'H')
    dot.edge('H', 'I')

    # Render flowchart
    dot.render('flowchart', format='png', cleanup=True)
    print("Flowchart saved as flowchart.png")

draw_flowchart()
